// -----------------------------------------------------------------------------
//
// Copyright (C) The BioDynaMo Project, Liudmila Rozanova, Alexander Temerev
// and Jean de Montigny
// All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
//
// See the LICENSE file distributed with this work for details.
// See the NOTICE file distributed with this work for additional information
// regarding copyright ownership.
//
// -----------------------------------------------------------------------------
#include "openlb_sim.h"
#include "olb3D.h"
#include <vector>
#include <cmath>
#include <iostream>
#include <fstream>
#include "biodynamo.h"
#include "human.h"
#include "sim-param.h"

namespace openlb_sim {

using namespace olb;
using namespace olb::descriptors;
using namespace olb::graphics;
using namespace olb::util;

typedef double T;

// Choose your turbulent model of choice
//#define RLB
#define Smagorinsky //default
//#define ConsitentStrainSmagorinsky
//#define ShearSmagorinsky
//#define Krause

#ifdef ShearSmagorinsky
#define DESCRIPTOR ShearSmagorinskyD3Q19Descriptor
#else
#define DESCRIPTOR D3Q19<>
#endif

// ---------------------------------------------------------------------------
// Parameters for the simulation setup

// resolution of the model: number of voxel per physLength
const int N = 2;
// time discretization refinement
const int M = 1;
// block profile (mode=0), power profile (mode=1)
const int inflowProfileMode = 0;
// max. simulation time
const T maxPhysT = 0.0001;

// ---------------------------------------------------------------------------
template <typename T, typename _DESCRIPTOR>
class TurbulentVelocity3D : public AnalyticalF3D<T,T> {

protected:
  // block profile (mode=0), power profile (mode=1)
  int _mode;
  T rho;
  T nu;
  T u0;
  T p0;
  T charL;
  T dx;

public:
  TurbulentVelocity3D( UnitConverter<T,_DESCRIPTOR> const& converter, int mode=0 ) : AnalyticalF3D<T,T>( 3 ) {
    _mode = mode;
    u0 = converter.getCharLatticeVelocity();
    rho = converter.getPhysDensity();
    nu = converter.getPhysViscosity();
    charL = converter.getCharPhysLength();
    p0 = converter.getCharPhysPressure();
    dx = converter.getConversionFactorLength();

    this->getName() = "turbulentVelocity3d";
  };

  bool operator()( T output[], const S input[] ) override {
    T y = input[1];
    T z = input[2];
    // block profile inititalization
    T u_calc = u0;
    // power profile inititalization
    if ( _mode==1 ) {
      T obst_y = 5.5+dx;
      T obst_z = 5.5+dx;
      T obst_r = 0.5;

      T B      = 5.5;
      T kappa  = 0.4;
      T ReTau  = 183.6;

      u_calc = u0/7.*( 2.*nu*ReTau/( charL*kappa )*log( fabs( 2.*ReTau/charL*( obst_r - sqrt( pow( y - obst_y, 2. )
                       + pow( z - obst_z, 2. ) ) )*1.5*( 1 + sqrt( pow( y - obst_y, 2. )
                           + pow( z - obst_z, 2. ) )/obst_r )/( 1 + 2.*pow( sqrt( pow( y - obst_y, 2. )
                               + pow( z - obst_z, 2. ) )/obst_r, 2. ) ) ) + B ) );
    }
    T a = -1., b = 1.;
    T nRandom = rand()/( T )RAND_MAX*( b-a ) + a;

    output[0] = u_calc+  0.15*u0*nRandom;
    output[1] = 0.15*u0*nRandom;
    output[2] = 0.15*u0*nRandom;
    return true;
  };
};

// ---------------------------------------------------------------------------
void prepareGeometry( UnitConverter<T,DESCRIPTOR> const& converter,
                      IndicatorF3D<T>& indicator, STLreader<T>& stlReader,
                      SuperGeometry3D<T>& superGeometry ) {

  OstreamManager clout( std::cout,"prepareGeometry" );
  clout << "Prepare Geometry ..." << std::endl;

  // Sets material number for fluid and boundary
  superGeometry.rename( 0, 2, indicator );
  superGeometry.rename( 0, 1 );
  superGeometry.rename( 2, 0, stlReader );

  superGeometry.clean();

  // -------- biodynamo -------- //
  auto* sim = bdm::Simulation::GetActive();
  auto* rm = sim->GetResourceManager();
  auto* sparam = sim->GetParam()->GetModuleParam<bdm::SimParam>();
  double radius = sparam->human_diameter/2;

  std::vector<bdm::Double3> agents_position;
  std::vector<std::vector<double>> agents_direction;
  std::vector<int> agents_state;
  // get humans info
  auto get_agents_lists = [&agents_position, &agents_direction,
    &agents_state](bdm::SimObject* so) {
    auto* hu = bdm::bdm_static_cast<bdm::Human*>(so);
    agents_position.push_back(hu->GetPosition());
    agents_direction.push_back(hu->orientation_);
    agents_state.push_back(hu->state_);
  };
  rm->ApplyOnAllElements(get_agents_lists);

  for (size_t agent = 0; agent < agents_position.size(); agent++ ) {
    // NOTE: spread only for infected agents
    if (agents_state[agent] == bdm::State::kInfected) {
      bdm::Double3 pos = agents_position[agent];
      auto dir = agents_direction[agent];

      // -------- create inlets -------- //
      Vector<T,3> spread_pos_in(
        (pos[0] + dir[0]*radius+1)*converter.getCharPhysLength(),
        (pos[1] + dir[1]*radius+1)*converter.getCharPhysLength(),
        pos[2] );
      Vector<T,3> spread_pos_out(
        (pos[0] + dir[0]*(radius+3))*converter.getCharPhysLength(),
        (pos[1] + dir[1]*(radius+3))*converter.getCharPhysLength(),
        pos[2] );

      IndicatorCylinder3D<T> layerInflow( spread_pos_in, spread_pos_out,
        1*converter.getCharPhysLength());
      superGeometry.rename( 1, 3, layerInflow );
    } // if agent is infected
  } // for each agent in sim

  // Removes all not needed boundary voxels outside the surface
  superGeometry.clean();
  // Removes all not needed boundary voxels inside the surface
  superGeometry.innerClean();
  superGeometry.checkForErrors();

  superGeometry.print();
  clout << "Prepare Geometry ... OK" << std::endl;
}

// ---------------------------------------------------------------------------
void prepareLattice( SuperLattice3D<T,DESCRIPTOR>& sLattice,
                     UnitConverter<T,DESCRIPTOR> const& converter,
                     Dynamics<T, DESCRIPTOR>& bulkDynamics,
                     sOnLatticeBoundaryCondition3D<T,DESCRIPTOR>& bc,
                     sOffLatticeBoundaryCondition3D<T,DESCRIPTOR>& offBc,
                     STLreader<T>& stlReader,
                     SuperGeometry3D<T>& superGeometry ) {



  OstreamManager clout( std::cout,"prepareLattice" );
  clout << "Prepare Lattice ..." << std::endl;

  const T omega = converter.getLatticeRelaxationFrequency();

  // Material=0 -->do nothing
  sLattice.defineDynamics(
    superGeometry, 0, &instances::getNoDynamics<T, DESCRIPTOR>() );

  // Material=1 -->bulk dynamics
  sLattice.defineDynamics( superGeometry, 1, &bulkDynamics );

  // Material=2 -->bounce back
  sLattice.defineDynamics(
    superGeometry, 2, &instances::getBounceBack<T, DESCRIPTOR>() );

  // Material=3 -->bulk dynamics (inflow)
  sLattice.defineDynamics( superGeometry, 3, &bulkDynamics );
  bc.addVelocityBoundary( superGeometry, 3, omega );

  clout << "Prepare Lattice ... OK" << std::endl;
}

// ---------------------------------------------------------------------------
void setBoundaryValues( UnitConverter<T,DESCRIPTOR> const&converter,
                        SuperLattice3D<T,DESCRIPTOR>& lattice,
                        SuperGeometry3D<T>& superGeometry, int iT ) {

  OstreamManager clout( std::cout,"setBoundaryValues" );

  if ( iT==0 ) {
    AnalyticalConst3D<T,T> rhoF( 1 );
    std::vector<T> velocity( 3,T() );
    AnalyticalConst3D<T,T> uF( velocity );

    // Seeding of random fluctuations and definition of the velocity field
    srand( time( nullptr ) );
    TurbulentVelocity3D<T,DESCRIPTOR> uSol( converter, inflowProfileMode );

    lattice.iniEquilibrium(
      // superGeometry.getMaterialIndicator({1, 2, 4}), rhoF, uF );
      superGeometry.getMaterialIndicator({1, 2}), rhoF, uF );
    lattice.iniEquilibrium( superGeometry, 3, rhoF, uSol );

    lattice.defineU( superGeometry, 3, uSol );
    // lattice.defineRho( superGeometry, 4, rhoF );

    // Make the lattice ready for simulation
    lattice.initialize();
  }
}

// ---------------------------------------------------------------------------
void getResults( SuperLattice3D<T, DESCRIPTOR>& sLattice,
                 UnitConverter<T,DESCRIPTOR> const& converter, int iT,
                 SuperGeometry3D<T>& superGeometry, Timer<T>& timer ) {

  OstreamManager clout( std::cout,"getResults" );
  SuperVTMwriter3D<T> vtmWriter( "bus_spreading" );

  if ( iT==0 ) {
    // Writes the geometry, cuboid no. and rank no. as vti file for visualization
    SuperLatticeGeometry3D<T, DESCRIPTOR> geometry( sLattice, superGeometry );
    SuperLatticeCuboid3D<T, DESCRIPTOR> cuboid( sLattice );
    SuperLatticeRank3D<T, DESCRIPTOR> rank( sLattice );
    vtmWriter.write( geometry );
    // vtmWriter.write( cuboid );
    // vtmWriter.write( rank );
    vtmWriter.createMasterFile();
  }

  // Writes the vtk files
  if ( iT%converter.getLatticeTime( maxPhysT/100. )==0 ) {
    // Create the data-reading functors...
    SuperLatticePhysVelocity3D<T, DESCRIPTOR> velocity( sLattice, converter );
    SuperLatticePhysPressure3D<T, DESCRIPTOR> pressure( sLattice, converter );
    vtmWriter.addFunctor( velocity );
    vtmWriter.addFunctor( pressure );
    vtmWriter.write( iT );
  }
}

// ---------------------------------------------------------------------------
int main( int argc, char* argv[] ) {

  // === 1st Step: Initialization ===
  olbInit( &argc, &argv );
  //TODO: add openlb sim number in output dir?
  singleton::directories().setOutputDir( "./output/openlb/" );
  OstreamManager clout( std::cout,"main" );
  // display messages from every single mpi process
  // clout.setMultiOutput(true);

  UnitConverterFromResolutionAndRelaxationTime<T, DESCRIPTOR> const converter(
    int {N},        // resolution: number of voxels per charPhysL
    (T)   0.500018, // latticeRelaxationTime: relaxation time, have to be greater than 0.5!
    (T)   0.01,     // charPhysLength: reference length of simulation geometry
    (T)   1,        // charPhysVelocity: maximal/highest expected velocity during simulation in __m / s__
    (T)   0.0002,   // physViscosity: physical kinematic viscosity in __m^2 / s__
    (T)   1.0       // physDensity: physical density in __kg / m^3__
  );
  // Prints the converter log as console output
  converter.print();
  // Writes the converter log in a file
  converter.write("bus_spreading");

  // === 2nd Step: Prepare Geometry ===
  STLreader<T> stlReader( "./output/openlb/bus.stl",
    converter.getConversionFactorLength(), 0.01 );
  IndicatorLayer3D<T> extendedDomain( stlReader,
    converter.getConversionFactorLength() );

  // Instantiation of a cuboidGeometry with weights
  #ifdef PARALLEL_MODE_MPI
    const int noOfCuboids = std::min( 16*N, 2*singleton::mpi().getSize() );
  #else
    const int noOfCuboids = 2;
  #endif
    CuboidGeometry3D<T> cuboidGeometry( extendedDomain,
      converter.getConversionFactorLength(), noOfCuboids );

    // Instantiation of a loadBalancer
    HeuristicLoadBalancer<T> loadBalancer( cuboidGeometry );

    // Instantiation of a superGeometry
    SuperGeometry3D<T> superGeometry( cuboidGeometry, loadBalancer, 2 );

    prepareGeometry( converter, extendedDomain, stlReader, superGeometry );

  // === 3rd Step: Prepare Lattice ===
  SuperLattice3D<T, DESCRIPTOR> sLattice( superGeometry );

  Dynamics<T, DESCRIPTOR>* bulkDynamics;
  const T omega = converter.getLatticeRelaxationFrequency();
#if defined(RLB)
  bulkDynamics = new RLBdynamics<T, DESCRIPTOR>(
    omega, instances::getBulkMomenta<T, DESCRIPTOR>() );
#elif defined(Smagorinsky)
  bulkDynamics = new SmagorinskyBGKdynamics<T, DESCRIPTOR>(
    omega, instances::getBulkMomenta<T, DESCRIPTOR>(),
      0.15);
#elif defined(ShearSmagorinsky)
  bulkDynamics = new ShearSmagorinskyBGKdynamics<T, DESCRIPTOR>(
    omega, instances::getBulkMomenta<T, DESCRIPTOR>(),
      0.15);
#elif defined(Krause)
  bulkDynamics = new KrauseBGKdynamics<T, DESCRIPTOR>(
    omega, instances::getBulkMomenta<T, DESCRIPTOR>(),
      0.15);
#else //ConsitentStrainSmagorinsky
  bulkDynamics = new ConStrainSmagorinskyBGKdynamics<T, DESCRIPTOR>(
    omega, instances::getBulkMomenta<T, DESCRIPTOR>(),
      0.05);
#endif

  sOnLatticeBoundaryCondition3D<T, DESCRIPTOR> sBoundaryCondition(sLattice);
  createInterpBoundaryCondition3D<T, DESCRIPTOR> ( sBoundaryCondition );

  sOffLatticeBoundaryCondition3D<T, DESCRIPTOR> sOffBoundaryCondition(sLattice);
  createBouzidiBoundaryCondition3D<T, DESCRIPTOR> ( sOffBoundaryCondition );

  prepareLattice( sLattice, converter, *bulkDynamics, sBoundaryCondition,
    sOffBoundaryCondition, stlReader, superGeometry );

  // === 4th Step: Main Loop with Timer ===
  Timer<T> timer( converter.getLatticeTime( maxPhysT ),
    superGeometry.getStatistics().getNvoxel() );
  timer.start();

  OstreamManager cloutsim( std::cout,"openlb simulation" );
  cloutsim << "Turbulence simulation ..." << std::endl;

  int max_step = converter.getLatticeTime( maxPhysT );
  int print_step = (int)max_step/5;

  for ( int iT = 0; iT <= max_step; ++iT ) {

    if (iT % print_step == 0) {
      cloutsim << "Step " << iT << " out of " << max_step << std::endl;
    }

    // === 5ath Step: Apply filter
#ifdef ADM
    SuperLatticeADM3D<T, DESCRIPTOR> admF( sLattice, 0.01, 2 );
    admF.execute( superGeometry, 1 );
#endif

    // === 5bth Step: Definition of Initial and Boundary Conditions ===
    setBoundaryValues( converter, sLattice, superGeometry, iT );

    // === 6th Step: Collide and Stream Execution ===
    sLattice.collideAndStream();

    // === 7th Step: Computation and Output of the Results ===
    getResults( sLattice, converter, iT, superGeometry, timer );
  } // end for getLatticeTime
  cloutsim << "Turbulence simulation ... OK" << std::endl;

  timer.stop();
  timer.printSummary();

  // -------- biodynamo -------- //
  // update virus concentration at each agent (spread) position
  auto* sim = bdm::Simulation::GetActive();
  auto* rm = sim->GetResourceManager();
  auto* sparam = sim->GetParam()->GetModuleParam<bdm::SimParam>();
  double radius = sparam->human_diameter/2;
  auto update_agents_virus_concentration = [&radius, &converter]
    (bdm::SimObject* so) {
    auto* hu = bdm::bdm_static_cast<bdm::Human*>(so);
    if (hu->state_ == bdm::State::kHealthy) {
      bdm::Double3 pos = hu->GetPosition();
      std::vector<double> dir = hu->orientation_;
      bdm::Double3 spread_pos = {
        (pos[0] + dir[0]*radius)*converter.getCharPhysLength(),
        (pos[1] + dir[1]*radius)*converter.getCharPhysLength(),
        pos[2]*converter.getCharPhysLength() };
      // hu->virus_concentration_ = GetVirusConcentration(spread_pos);
    } // end if kHealthy
  }; // end for each agents in sim
  rm->ApplyOnAllElements(update_agents_virus_concentration);

  delete bulkDynamics;

  return 1;
} // end main

} // namespace openlb_sim
