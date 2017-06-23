/*    Copyright (c) 2010-2017, Delft University of Technology
 *    All rigths reserved
 *
 *    This file is part of the Tudat. Redistribution and use in source and
 *    binary forms, with or without modification, are permitted exclusively
 *    under the terms of the Modified BSD license. You should have received
 *    a copy of the license with this file. If not, please or visit:
 *    http://tudat.tudelft.nl/LICENSE.
 */

#ifndef TUDAT_SETNUMERICALLYINTEGRATEDSTATES_H
#define TUDAT_SETNUMERICALLYINTEGRATEDSTATES_H

#include "Tudat/Basics/utilities.h"
#include "Tudat/SimulationSetup/EnvironmentSetup/body.h"
#include "Tudat/Astrodynamics/Ephemerides/frameManager.h"
#include "Tudat/Astrodynamics/Ephemerides/multiArcEphemeris.h"
#include "Tudat/Astrodynamics/Ephemerides/tabulatedEphemeris.h"
#include "Tudat/Astrodynamics/Ephemerides/tabulatedRotationalEphemeris.h"
#include "Tudat/SimulationSetup/PropagationSetup/propagationSettings.h"


namespace tudat
{

namespace propagators
{


//! Function to create an interpolator for the new translational state of a body.
/*!
 * Function to create an interpolator for the new translational state of a body.
 * \param stateMap New state history, w.r.t. the required ephemeris origin.
 * \return Lagrange interpolator (order 6) that produces the required continuous state.
 */
template< typename TimeType, typename StateScalarType >
boost::shared_ptr< interpolators::OneDimensionalInterpolator< TimeType, Eigen::Matrix< StateScalarType, 6, 1 > > >
createStateInterpolator(
        const std::map< TimeType, Eigen::Matrix< StateScalarType, 6, 1 > >& stateMap );

//! Function to reset the tabulated ephemeris of a body
/*!
 * Function to reset the tabulated ephemeris of a body
 * \param ephemerisInput New state history that is to be set
 * \param tabulatedEphemeris Ephemeris in which the ephemerisInput is to be set.
 */
template< typename StateTimeType, typename StateScalarType, typename EphemerisTimeType, typename EphemerisScalarType  >
void resetIntegratedEphemerisOfBody(
        const std::map< StateTimeType, Eigen::Matrix< StateScalarType, 6, 1 > >& ephemerisInput,
        const boost::shared_ptr< ephemerides::TabulatedCartesianEphemeris< EphemerisScalarType, EphemerisTimeType > > tabulatedEphemeris )
{
    std::map< EphemerisTimeType, Eigen::Matrix< EphemerisScalarType, 6, 1 > > castEphemerisInput;
    utilities::castMatrixMap< StateTimeType, StateScalarType, EphemerisTimeType, EphemerisScalarType, 6, 1 >(
                ephemerisInput, castEphemerisInput );

    boost::shared_ptr< interpolators::OneDimensionalInterpolator< EphemerisTimeType, Eigen::Matrix< EphemerisScalarType, 6, 1 > > >
            ephemerisInterpolator = createStateInterpolator( castEphemerisInput );
    tabulatedEphemeris->resetInterpolator( ephemerisInterpolator );
}

//! Function to reset the tabulated ephemeris of a body
/*!
 * Function to reset the tabulated ephemeris of a body, this requires the requested body to possess
 * an ephemeris of type TabulatedCartesianEphemeris< StateScalarType, TimeType >
 * \param bodyMap List of bodies used in simulations.
 * \param ephemerisInput New state history of the body
 * \param bodyToIntegrate Name of body for which the ephemeris is to be reset.
 */
template< typename TimeType, typename StateScalarType >
void resetIntegratedEphemerisOfBody(
        const simulation_setup::NamedBodyMap& bodyMap,
        const std::map< TimeType, Eigen::Matrix< StateScalarType, 6, 1 > >& ephemerisInput,
        const std::string& bodyToIntegrate )
{
    using namespace tudat::interpolators;
    using namespace tudat::ephemerides;

    // If body does not have ephemeris, give error message.
    if( bodyMap.at( bodyToIntegrate )->getEphemeris( ) == NULL )
    {
        throw std::runtime_error( "Error when resetting integrated ephemeris of body " +
                                  bodyToIntegrate + "no ephemeris found" );
    }

    // If current ephemeris is not already a tabulated ephemeris, give error message.
    else if( !isTabulatedEphemeris( bodyMap.at( bodyToIntegrate )->getEphemeris( ) ) )
    {
        throw std::runtime_error( "Error when resetting integrated ephemeris of body " +
                                  bodyToIntegrate + " no tabulated ephemeris found" );

    }
    // Else, update existing tabulated ephemeris
    else
    {
        std::cerr<<"Warning, tabulated ephemeris is being reset using data at different precision"<<std::endl;
        if( boost::dynamic_pointer_cast< TabulatedCartesianEphemeris< StateScalarType, TimeType > >(
                    bodyMap.at( bodyToIntegrate )->getEphemeris( ) ) != NULL )
        {
            boost::shared_ptr< OneDimensionalInterpolator< TimeType, Eigen::Matrix< StateScalarType, 6, 1 > > >
                    ephemerisInterpolator = createStateInterpolator( ephemerisInput );
            boost::shared_ptr< TabulatedCartesianEphemeris< StateScalarType, TimeType > > tabulatedEphemeris =
                    boost::dynamic_pointer_cast< TabulatedCartesianEphemeris< StateScalarType, TimeType > >(
                        bodyMap.at( bodyToIntegrate )->getEphemeris( ) );
            tabulatedEphemeris->resetInterpolator( ephemerisInterpolator );
        }
        else
        {
            if( boost::dynamic_pointer_cast< TabulatedCartesianEphemeris< double, double > >(
                        bodyMap.at( bodyToIntegrate )->getEphemeris( ) ) != NULL )
            {
                resetIntegratedEphemerisOfBody(
                            ephemerisInput, boost::dynamic_pointer_cast< TabulatedCartesianEphemeris< double, double > >(
                                bodyMap.at( bodyToIntegrate )->getEphemeris( ) ) );
            }
            else if( boost::dynamic_pointer_cast< TabulatedCartesianEphemeris< long double, double > >(
                         bodyMap.at( bodyToIntegrate )->getEphemeris( ) ) != NULL )
            {
                resetIntegratedEphemerisOfBody(
                            ephemerisInput, boost::dynamic_pointer_cast< TabulatedCartesianEphemeris< long double, double > >(
                                bodyMap.at( bodyToIntegrate )->getEphemeris( ) ) );
            }
            else if( boost::dynamic_pointer_cast< TabulatedCartesianEphemeris< double, Time > >(
                         bodyMap.at( bodyToIntegrate )->getEphemeris( ) ) != NULL )
            {
                resetIntegratedEphemerisOfBody(
                            ephemerisInput, boost::dynamic_pointer_cast< TabulatedCartesianEphemeris< double, Time > >(
                                bodyMap.at( bodyToIntegrate )->getEphemeris( ) ) );
            }
            else if( boost::dynamic_pointer_cast< TabulatedCartesianEphemeris< long double, Time > >(
                         bodyMap.at( bodyToIntegrate )->getEphemeris( ) ) != NULL )
            {
                resetIntegratedEphemerisOfBody(
                            ephemerisInput, boost::dynamic_pointer_cast< TabulatedCartesianEphemeris< long double, Time > >(
                                bodyMap.at( bodyToIntegrate )->getEphemeris( ) ) );
            }
            else
            {
                throw std::runtime_error( "Error, no tabulated ephemeris found when resetting ephemeris" );
            }
        }
    }
}

//! Function to convert output of translational motion to input for the ephemeris.
/*!
 * Function to convert output of translational motion from the numerical integrator to the required
 * input for the ephemeris.  It extracts the state history of a single body from the full list of
 * integrated states Additionally, it changes the origin of the reference frame in which the states
 * are given, by using the integrationToEphemerisFrameFunction input variable.
 * \param bodyIndex Index of integrated body for which the state is to be retrieved
 * \param startIndex Index in entries of equationsOfMotionNumericalSolution where the translational states start.
 * \param equationsOfMotionNumericalSolution Full numerical solution of numerical integrator,
 * already converted to Cartesian states (w.r.t. the integration origin of the body of bodyIndex)
 * \param integrationToEphemerisFrameFunction Function to provide the state of the ephemeris origin
 * of the current body w.r.t. its integration origin.
 * \return State history of body bodyIndex w.r.t. the origin with which its ephemeris is defined.
*/
template< typename TimeType, typename StateScalarType >
std::map< TimeType, Eigen::Matrix< StateScalarType, 6, 1 > > convertNumericalSolutionToEphemerisInput(
        const int bodyIndex,
        const int startIndex,
        const std::map< TimeType, Eigen::Matrix< StateScalarType, Eigen::Dynamic, 1 > >&
        equationsOfMotionNumericalSolution,
        const boost::function< Eigen::Matrix< StateScalarType, 6, 1 >( const TimeType ) >
        integrationToEphemerisFrameFunction = NULL )
{
    std::map< TimeType, Eigen::Matrix< StateScalarType, 6, 1 > > ephemerisTable;

    // If no integrationToEphemerisFrameFunction is provided, origin is already correct; only
    // extract required indices.
    if( integrationToEphemerisFrameFunction == 0 )
    {
        for( typename std::map< TimeType, Eigen::Matrix< StateScalarType, Eigen::Dynamic, 1 > >::const_iterator
             bodyIterator = equationsOfMotionNumericalSolution.begin( );
             bodyIterator != equationsOfMotionNumericalSolution.end( ); bodyIterator++ )
        {
            ephemerisTable[ bodyIterator->first ] = bodyIterator->second.block( startIndex + 6 * bodyIndex, 0, 6, 1 );
        }
    }
    // Else, extract indices and add required translation from integrationToEphemerisFrameFunction
    else
    {
        for( typename std::map< TimeType, Eigen::Matrix< StateScalarType, Eigen::Dynamic, 1 > >::const_iterator
             bodyIterator = equationsOfMotionNumericalSolution.begin( );
             bodyIterator != equationsOfMotionNumericalSolution.end( ); bodyIterator++ )
        {

            ephemerisTable[ bodyIterator->first ] = bodyIterator->second.block( startIndex + 6 * bodyIndex, 0, 6, 1 ) -
                    integrationToEphemerisFrameFunction( bodyIterator->first );
        }
    }
    return ephemerisTable;
}

template< typename TimeType, typename StateScalarType >
void getSingleBodyStateHistoryFromPropagationOutpiut(
        const std::vector< std::string >& bodiesToIntegrate,
        const int translationalStateStartIndex,
        const std::string& bodyForWhichToRetrieveState,
        const std::map< TimeType, Eigen::Matrix< StateScalarType, Eigen::Dynamic, 1 > >& equationsOfMotionNumericalSolution,
        std::map< TimeType, Eigen::Matrix< StateScalarType, 6, 1 > >& ephemerisInput,
        int& bodyIndex,
        const std::map< std::string, boost::function< Eigen::Matrix< StateScalarType, 6, 1 >( const TimeType ) > >&
        integrationToEphemerisFrameFunctions =
        std::map< std::string, boost::function< Eigen::Matrix< StateScalarType, 6, 1 >( const TimeType ) > >( ) )
{
    // Get index of current body to be updated in bodiesToIntegrate.
    std::vector< std::string >::const_iterator bodyFindIterator = std::find(
                bodiesToIntegrate.begin( ), bodiesToIntegrate.end( ), bodyForWhichToRetrieveState );
    if( bodyFindIterator == bodiesToIntegrate.end( ) )
    {
        throw std::runtime_error( "Error when creating and setting ephemeris after integration, cannot find body " +
                                  bodyForWhichToRetrieveState );
    }
    bodyIndex = std::distance( bodiesToIntegrate.begin( ), bodyFindIterator );

    // Get frame origin function if applicable
    boost::function< Eigen::Matrix< StateScalarType, 6, 1 >( const TimeType ) > integrationToEphemerisFrameFunction =
            NULL;
    if( integrationToEphemerisFrameFunctions.count( bodiesToIntegrate.at( bodyIndex ) ) > 0 )
    {
        integrationToEphemerisFrameFunction =
                integrationToEphemerisFrameFunctions.at( bodiesToIntegrate.at( bodyIndex ) );
    }

    // Create and reset interpolator.
    ephemerisInput = convertNumericalSolutionToEphemerisInput(
                bodyIndex, translationalStateStartIndex, equationsOfMotionNumericalSolution, integrationToEphemerisFrameFunction );
}

//! Create and reset ephemerides interpolator
/*!
 * Creates and resets the interpolator for the ephemerides of the integrated bodies from the
 * numerical integration results.
 * \param bodyMap List of bodies used in simulations.
 * \param bodiesToIntegrate List of names of bodies which are numericall integrated (in the order in
 * which they are in the equationsOfMotionNumericalSolution map.
 * \param startIndex Index in entries of equationsOfMotionNumericalSolution where the translational states start.
 * \param ephemerisUpdateOrder Order in which to update the ephemeris objects.
 * \param equationsOfMotionNumericalSolution Numerical solution of translational equations of
 * motion, in Cartesian elements w.r.t. integratation origins.
 * \param integrationToEphemerisFrameFunctions Function to provide the states of the ephemeris
 * origins of each body w.r.t. their respective integration origins.
 */
template< typename TimeType, typename StateScalarType >
void createAndSetInterpolatorsForEphemerides(
        const simulation_setup::NamedBodyMap& bodyMap,
        const std::vector< std::string >& bodiesToIntegrate,
        const int startIndex,
        const std::vector< std::string >& ephemerisUpdateOrder,
        const std::map< TimeType, Eigen::Matrix< StateScalarType, Eigen::Dynamic, 1 > >& equationsOfMotionNumericalSolution,
        const std::map< std::string, boost::function< Eigen::Matrix< StateScalarType, 6, 1 >( const TimeType ) > >&
        integrationToEphemerisFrameFunctions =
        std::map< std::string, boost::function< Eigen::Matrix< StateScalarType, 6, 1 >( const TimeType ) > >( ) )
{
    using namespace tudat::interpolators;

    std::map< TimeType, Eigen::Matrix< StateScalarType, 6, 1 > > ephemerisInput;
    int bodyIndex;

    // Iterate over all bodies that are integrated numerically and create state interpolator.
    for( unsigned int i = 0; i < ephemerisUpdateOrder.size( ); i++ )
    {
        ephemerisInput.clear( );
        getSingleBodyStateHistoryFromPropagationOutpiut(
                    bodiesToIntegrate, startIndex, ephemerisUpdateOrder.at( i ), equationsOfMotionNumericalSolution,
                    ephemerisInput, bodyIndex, integrationToEphemerisFrameFunctions );
        resetIntegratedEphemerisOfBody(
                    bodyMap, ephemerisInput, bodiesToIntegrate.at( bodyIndex ) );
    }
}

//! Resets the ephemerides of the integrated bodies from the numerical integration results.
/*!
 * Resets the ephemerides of the integrated bodies from the numerical integration results, and
 * performs associated computation for ephemeris-dependent environment variables.
 * \param bodyMap List of bodies used in simulations.
 * \param bodiesToIntegrate List of names of bodies which are numerically integrated (in the order in
 * which they are in the equationsOfMotionNumericalSolution map.
 * \param startIndexAndSize Pair with start index and total (contiguous) size of integrated states in entries of
 * equationsOfMotionNumericalSolution
 * \param ephemerisUpdateOrder Order in which to update the ephemeris objects (empty if arbitrary).
 * \param equationsOfMotionNumericalSolution Numerical solution of translational equations of
 * motion, in Cartesian elements w.r.t. integratation origins.
 * \param integrationToEphemerisFrameFunctions Function to provide the states of the ephemeris
 * origins of each body w.r.t. their respective integration origins.
 */
template< typename TimeType, typename StateScalarType >
void resetIntegratedEphemerides(
        const simulation_setup::NamedBodyMap& bodyMap,
        const std::map< TimeType, Eigen::Matrix< StateScalarType, Eigen::Dynamic, 1 > >& equationsOfMotionNumericalSolution,
        const std::vector< std::string >& bodiesToIntegrate,
        const std::pair< unsigned int, unsigned int > startIndexAndSize,
        std::vector< std::string > ephemerisUpdateOrder = std::vector< std::string >( ),
        const std::map< std::string, boost::function< Eigen::Matrix< StateScalarType, 6, 1 >( const TimeType ) > >&
        integrationToEphemerisFrameFunctions =
        std::map< std::string, boost::function< Eigen::Matrix< StateScalarType, 6, 1 >( const TimeType ) > >( ) )
{
    // Set update order arbitrarily if no order is provided.
    if( ephemerisUpdateOrder.size( ) == 0 )
    {
        ephemerisUpdateOrder = bodiesToIntegrate;
    }
    // Check input consistency
    else if( ephemerisUpdateOrder.size( ) != bodiesToIntegrate.size( ) )
    {
        throw std::runtime_error( "Error when resetting ephemerides, input vectors have inconsistent size" );
    }

    if( static_cast< unsigned int >( equationsOfMotionNumericalSolution.begin( )->second.rows( ) )
            < startIndexAndSize.first + startIndexAndSize.second )
    {
        throw std::runtime_error( "Error when resetting ephemerides, input solution inconsistent with start index and size." );
    }

    if( startIndexAndSize.second != 6 * bodiesToIntegrate.size( ) )
    {
        throw std::runtime_error( "Error when resetting ephemerides, number of bodies inconsistent with input size." );

    }

    // Create interpolators from numerical integration results (states) at discrete times.
    createAndSetInterpolatorsForEphemerides(
                bodyMap, bodiesToIntegrate, startIndexAndSize.first, ephemerisUpdateOrder,
                equationsOfMotionNumericalSolution, integrationToEphemerisFrameFunctions );
}


template< typename TimeType, typename StateScalarType >
void resetMultiArcIntegratedEphemerides(
        const simulation_setup::NamedBodyMap& bodyMap,
        const std::vector< std::map< TimeType, Eigen::Matrix< StateScalarType, Eigen::Dynamic, 1 > > >& equationsOfMotionNumericalSolution,
        const std::vector< double > arcStartTimes,
        const std::vector< std::string >& bodiesToIntegrate,
        const std::pair< unsigned int, unsigned int > startIndexAndSize,
        std::vector< std::string > ephemerisUpdateOrder = std::vector< std::string >( ),
        const std::map< std::string, boost::function< Eigen::Matrix< StateScalarType, 6, 1 >( const TimeType ) > >& integrationToEphemerisFrameFunctions =
        std::map< std::string, boost::function< Eigen::Matrix< StateScalarType, 6, 1 >( const TimeType ) > >( ) )
{
    using namespace tudat::interpolators;
    using namespace tudat::ephemerides;

    if( ephemerisUpdateOrder.size( ) == 0 )
    {
        ephemerisUpdateOrder = bodiesToIntegrate;
    }

    for( unsigned int i = 0; i < ephemerisUpdateOrder.size( ); i++ )
    {
        std::vector< std::string >::const_iterator bodyFindIterator = std::find(
                    bodiesToIntegrate.begin( ), bodiesToIntegrate.end( ), ephemerisUpdateOrder.at( i ) );
        int bodyIndex = std::distance( bodiesToIntegrate.begin( ), bodyFindIterator );

        std::vector< boost::shared_ptr< Ephemeris > > arcEphemerisList;
        boost::shared_ptr< MultiArcEphemeris > currentBodyEphemeris =
                boost::dynamic_pointer_cast< MultiArcEphemeris >( bodyMap.at( bodiesToIntegrate.at( bodyIndex ) )->getEphemeris( ) ) ;

        if( currentBodyEphemeris == NULL )
        {
            std::cerr<<"Error when resetting ephemeris of body "<<bodiesToIntegrate.at( bodyIndex )<<
                       ", original ephemeris is of incompatible type"<<std::endl;
        }

        for( unsigned int j = 0; j < arcStartTimes.size( ); j++ )
        {
            boost::function< Eigen::Matrix< StateScalarType, 6, 1 >( const TimeType ) > integrationToEphemerisFrameFunction = NULL;
            if( integrationToEphemerisFrameFunctions.count( bodiesToIntegrate.at( bodyIndex ) ) > 0 )
            {
                integrationToEphemerisFrameFunction = integrationToEphemerisFrameFunctions.at( bodiesToIntegrate.at( bodyIndex ) );
            }

            // Create interpolator.
            boost::shared_ptr< OneDimensionalInterpolator< TimeType, Eigen::Matrix< StateScalarType, 6, 1 > > > ephemerisInterpolator =
                    createStateInterpolator( convertNumericalSolutionToEphemerisInput(
                                                 bodyIndex, startIndexAndSize.first,
                                                 equationsOfMotionNumericalSolution.at( j ), integrationToEphemerisFrameFunction ) );

            arcEphemerisList.push_back( boost::make_shared< TabulatedCartesianEphemeris< StateScalarType, TimeType > >(
                                            ephemerisInterpolator, currentBodyEphemeris->getReferenceFrameOrigin( ),
                                            currentBodyEphemeris->getReferenceFrameOrientation( ) ) );

        }

        currentBodyEphemeris->resetSingleArcEphemerides( arcEphemerisList, arcStartTimes );
    }

    // Having set new ephemerides, update body properties depending on ephemerides.
    for( simulation_setup::NamedBodyMap::const_iterator bodyIterator = bodyMap.begin( );
         bodyIterator != bodyMap.end( ); bodyIterator++ )
    {
        bodyIterator->second->updateConstantEphemerisDependentMemberQuantities( );
    }
}

template< typename TimeType, typename StateScalarType >
void resetIntegratedRotationalEphemerisOfBody(
        const simulation_setup::NamedBodyMap& bodyMap,
        const boost::shared_ptr< interpolators::OneDimensionalInterpolator< TimeType, Eigen::Matrix< StateScalarType, 7, 1 > > >
        rotationalEphemerisInterpolator,
        const std::string bodyToIntegrate )
{
    using namespace tudat::interpolators;
    using namespace tudat::ephemerides;

    if( bodyMap.at( bodyToIntegrate )->getRotationalEphemeris( ) == NULL )
    {
        std::cerr<<"Error, no rotational ephemeris detected for body "<<bodyToIntegrate<<" when resetting ephemeris"<<std::endl;
    }

    // If current ephemeris is not already a tabulated ephemeris, create new ephemeris.
    else if( boost::dynamic_pointer_cast< TabulatedRotationalEphemeris< StateScalarType, TimeType > >(
                 bodyMap.at( bodyToIntegrate )->getRotationalEphemeris( ) ) == NULL )
    {
        std::cerr<<"Error B when resetting integrated ephemeris of body "<<std::endl;

    }
    // Else, update existing tabulated ephemeris
    else
    {

        boost::shared_ptr< TabulatedRotationalEphemeris< StateScalarType, TimeType > > tabulatedEphemeris =
                boost::dynamic_pointer_cast< TabulatedRotationalEphemeris<  StateScalarType, TimeType > >(
                    bodyMap.at( bodyToIntegrate )->getRotationalEphemeris( ) );
        tabulatedEphemeris->reset( rotationalEphemerisInterpolator );
    }
}

// Set state history of current body from integration result for interpolator input.
template< typename TimeType, typename StateScalarType >
std::map< TimeType, Eigen::Matrix< StateScalarType, 7, 1 > > convertNumericalSolutionToRotationalEphemerisInput(
        const int bodyIndex,
        const std::map< TimeType, Eigen::Matrix< StateScalarType, Eigen::Dynamic, 1 > >& equationsOfMotionNumericalSolution )
{
    std::map< TimeType, Eigen::Matrix< StateScalarType, 7, 1 > > ephemerisTable;

    for( typename std::map< TimeType, Eigen::Matrix< StateScalarType, Eigen::Dynamic, 1 > >::const_iterator bodyIterator =
         equationsOfMotionNumericalSolution.begin( ); bodyIterator != equationsOfMotionNumericalSolution.end( ); bodyIterator++ )
    {
        ephemerisTable[ bodyIterator->first ] = bodyIterator->second.block( 7 * bodyIndex, 0, 7, 1 );
    }

    return ephemerisTable;
}

//! Function to create an interpolator for the new translational state of a body.
/*!
 * Function to create an interpolator for the new translational state of a body.
 * \param stateMap New state history, w.r.t. the required ephemeris origin.
 * \return Lagrange interpolator (order 6) that produces the required continuous state.
 */
template< typename TimeType, typename StateScalarType >
boost::shared_ptr< interpolators::OneDimensionalInterpolator< TimeType, Eigen::Matrix< StateScalarType, 6, 1 > > >
createStateInterpolator(
        const std::map< TimeType, Eigen::Matrix< StateScalarType, 6, 1 > >& stateMap );

//! Function to create an interpolator for the new rotational state of a body.
/*!
 * Function to create an interpolator for the new rotational state of a body.
 * \param stateMap New rotational state history.
 * \return Lagrange interpolator (order 6) that produces the required continuous rotational state.
 */
template< typename TimeType, typename StateScalarType >
boost::shared_ptr< interpolators::OneDimensionalInterpolator< TimeType, Eigen::Matrix< StateScalarType, 7, 1 > > >
createRotationalStateInterpolator(
        const std::map< TimeType, Eigen::Matrix< StateScalarType, 7, 1 > >& stateMap );


//! Function to reset the tabulated rotational ephemeris of a body
/*!
 * Function to reset the tabulated rotational ephemeris of a body
 * \param bodyMap List of bodies used in simulations.
 * \param bodiesToIntegrate List of names of bodies for which rotational state is numerically integrated
 * \param equationsOfMotionNumericalSolution New rotational state history that is to be set
 */
template< typename TimeType, typename StateScalarType >
void createAndSetInterpolatorsForRotationalEphemerides(
        const simulation_setup::NamedBodyMap& bodyMap,
        const std::vector< std::string >& bodiesToIntegrate,
        const std::map< TimeType, Eigen::Matrix< StateScalarType, Eigen::Dynamic, 1 > >& equationsOfMotionNumericalSolution )
{
    using namespace tudat::interpolators;

    // Iterate over all bodies that are integrated numerically and create state interpolator.
    for( unsigned int i = 0; i < bodiesToIntegrate.size( ); i++ )
    {
        // Create interpolator.
        boost::shared_ptr< OneDimensionalInterpolator< TimeType, Eigen::Matrix< StateScalarType, 7, 1 > > >
                ephemerisInterpolator = createRotationalStateInterpolator(
                    convertNumericalSolutionToRotationalEphemerisInput( i, equationsOfMotionNumericalSolution ) );

        resetIntegratedRotationalEphemerisOfBody( bodyMap, ephemerisInterpolator, bodiesToIntegrate.at( i ) );
    }
}


//! Resets the rotational ephemerides of a set of bodies from the numerical integration results.
/*!
 * Resets the rotational ephemerides of a set of bodies from the numerical integration results, and
 * performs associated computation for ephemeris-dependent environment variables.
 * \param bodyMap List of bodies used in simulations.
 * \param equationsOfMotionNumericalSolution Numerical solution of rotational equations of motion
 * \param bodiesToIntegrate List of names of bodies which are numerically integrated
 */
template< typename TimeType, typename StateScalarType >
void resetIntegratedRotationalEphemerides(
        const simulation_setup::NamedBodyMap& bodyMap,
        const std::map< TimeType, Eigen::Matrix< StateScalarType, Eigen::Dynamic, 1 > >& equationsOfMotionNumericalSolution,
        const std::vector< std::string >& bodiesToIntegrate )
{
    // Create interpolators from numerical integration results (states) at discrete times.
    createAndSetInterpolatorsForRotationalEphemerides(
                bodyMap, bodiesToIntegrate, equationsOfMotionNumericalSolution );

    // Having set new ephemerides, update body properties depending on ephemerides.
    for( simulation_setup::NamedBodyMap::const_iterator bodyIterator = bodyMap.begin( );
         bodyIterator != bodyMap.end( ); bodyIterator++ )
    {
        //NOTE: Inefficient, should be done once following full integration.
        bodyIterator->second->updateConstantEphemerisDependentMemberQuantities( );
    }
}

//! Resets the mass models of the integrated bodies from the numerical integration results.
/*!
 * Resets the mass models of the integrated bodies from the numerical integration results.
 * \param bodyMap List of bodies used in simulations.
 * \param equationsOfMotionNumericalSolution Numerical solution of the body masses.
 * \param bodiesToIntegrate List of names of bodies for which mass is numerically integrated (in the order in
 * which they are in the equationsOfMotionNumericalSolution map.
 * \param startIndexAndSize Pair with start index and total (contiguous) size of integrated states in entries of
 * equationsOfMotionNumericalSolution
 */
template< typename TimeType, typename StateScalarType >
void resetIntegratedBodyMass(
        const simulation_setup::NamedBodyMap& bodyMap,
        const std::map< TimeType, Eigen::Matrix< StateScalarType, Eigen::Dynamic, 1 > >& equationsOfMotionNumericalSolution,
        const std::vector< std::string >& bodiesToIntegrate ,
        const std::pair< unsigned int, unsigned int > startIndexAndSize )
{
    if( startIndexAndSize.second != bodiesToIntegrate.size( ) )
    {
        throw std::runtime_error( "Error when resetting body masses, number of bodies inconsistent with input size." );
    }

    // Iterate over all bodies for which mass is propagated.
    for( unsigned int i = 0; i < bodiesToIntegrate.size( ); i++ )
    {
        std::map< double, double > currentBodyMassMap;

        // Create mass map with double entries.
        for( typename std::map< TimeType, Eigen::Matrix< StateScalarType, Eigen::Dynamic, 1 > >::const_iterator
             stateIterator = equationsOfMotionNumericalSolution.begin( );
             stateIterator != equationsOfMotionNumericalSolution.end( ); stateIterator++ )
        {
            currentBodyMassMap[ static_cast< double >( stateIterator->first ) ] =
                    static_cast< double >( stateIterator->second( startIndexAndSize.first + i ) );
        }

        typedef interpolators::OneDimensionalInterpolator< double, double > LocalInterpolator;

        // Create and set interpolator.
        bodyMap.at( bodiesToIntegrate.at( i ) )->setBodyMassFunction( boost::bind(
                                                                          static_cast< double( LocalInterpolator::* )( const double ) >
                                                                          ( &LocalInterpolator::interpolate ),
                                                                          boost::make_shared< interpolators::LagrangeInterpolatorDouble >( currentBodyMassMap, 6 ), _1 ) );
    }
}

//! Base class for settings how numerically integrated states are processed
/*!
 *  Base class for defining settings on how numerically integrated states are to be processed in the
 *  environment. This class is pure virtual, and a derived class must be provided for each state
 *  type.
 */
template< typename TimeType, typename StateScalarType >
class IntegratedStateProcessor
{
public:

    //! Constructor
    /*!
     * Constructor
     * \param stateType Type of state that is to be set in environment.
     * \param startIndexAndSize Start index of current state type in integrated state vector, and
     * its size in the vector (first and second entry of pair).
     */
    IntegratedStateProcessor( const IntegratedStateType stateType,
                              const std::pair< int, int > startIndexAndSize ):
        stateType_( stateType ), startIndexAndSize_( startIndexAndSize ){ }

    //! Virtual destructor.
    virtual ~IntegratedStateProcessor( ){ }

    //! Function that processes the entries of the stateType_ in the full numericalSolution
    /*!
     * Function that processes the entries of the stateType_ in the full numericalSolution
     * \param numericalSolution Full numerical solution, in global representation (see
     * convertToOutputSolution function in associated SingleStateTypeDerivative derived class.
     */
    virtual void processIntegratedStates(
            const std::map< TimeType, Eigen::Matrix< StateScalarType,
            Eigen::Dynamic, 1 > >& numericalSolution ) = 0;

    virtual void processIntegratedMultiArcStates(
            const std::vector< std::map< TimeType, Eigen::Matrix< StateScalarType, Eigen::Dynamic, 1 > > >& numericalSolution,
            const std::vector< double >& arcStartTimes ) = 0;


    //! Type of state that is to be set in environment.
    IntegratedStateType stateType_;

    //! Start index of current state type in integrated state vector, and its size in the vector
    /*!
     *  Start index of current state type in integrated state vector, and its size in the vector
     * (first and second entry of pair).
     */
    std::pair< int, int > startIndexAndSize_;
};

//! Class used for processing numerically integrated translational states
/*!
 *  Class used for processing numerically integrated translational states, updates ephemeris object
 *  of each integrated body, performing frame translations if needed.
 */
template< typename TimeType, typename StateScalarType >
class TranslationalStateIntegratedStateProcessor: public IntegratedStateProcessor< TimeType, StateScalarType >
{
public:

    //! Constructor.
    /*!
     * Constructor.
     * \param startIndex Index in the state vector where the translational state starts.
     * \param bodyMap List of bodies used in simulations.
     * \param bodiesToIntegrate List of bodies for which the translational state is numerically
     * integrated. Order in this vector is the same as the order in state vector.
     * \param centralBodies List of origing w.r.t. which the translational states are integrated
     * (with the same order as bodiesToIntegrate).
     * \param frameManager Object to get state of one body w.r.t. another body.
     */
    TranslationalStateIntegratedStateProcessor(
            const int startIndex,
            const simulation_setup::NamedBodyMap& bodyMap,
            const std::vector< std::string >& bodiesToIntegrate,
            const std::vector< std::string >& centralBodies,
            const boost::shared_ptr< ephemerides::ReferenceFrameManager > frameManager ):
        IntegratedStateProcessor< TimeType, StateScalarType >(
            transational_state, std::make_pair( startIndex, 6 * bodiesToIntegrate.size( ) ) ),
        bodyMap_( bodyMap ), bodiesToIntegrate_( bodiesToIntegrate )
    {
        // Get update orders.
        ephemerisUpdateOrder_ = determineEphemerisUpdateorder(
                    bodiesToIntegrate_, centralBodies,
                    frameManager->getEphemerisOrigins( bodiesToIntegrate ) );

        // Get required frame origin translations.
        integrationToEphemerisFrameFunctions_
                = ephemerides::getTranslationFunctionsFromIntegrationFrameToEphemerisFrame
                < StateScalarType, TimeType >( centralBodies,
                                               bodiesToIntegrate_, frameManager );
    }

    ~TranslationalStateIntegratedStateProcessor( ){ }

    //! Function processing translational state in the full numericalSolution
    /*!
     * Function that processes the entries of the translational state in the full numericalSolution,
     * extracts and converts the states to the required frames, and updates the associated
     * ephemerides.
     * \param numericalSolution Full numerical solution, in global representation (see
     * convertToOutputSolution function in NBodyStateDerivative class.
     */
    void processIntegratedStates(
            const std::map< TimeType,
            Eigen::Matrix< StateScalarType, Eigen::Dynamic, 1 > >& numericalSolution )
    {
        resetIntegratedEphemerides< TimeType, StateScalarType >(
                    bodyMap_, numericalSolution, bodiesToIntegrate_, this->startIndexAndSize_, ephemerisUpdateOrder_,
                    integrationToEphemerisFrameFunctions_ );
    }

    void processIntegratedMultiArcStates(
            const std::vector< std::map< TimeType, Eigen::Matrix< StateScalarType, Eigen::Dynamic, 1 > > >& numericalSolution,
            const std::vector< double >& arcStartTimes )
    {
        resetMultiArcIntegratedEphemerides< TimeType, StateScalarType >(
                    bodyMap_, numericalSolution, arcStartTimes,
                    bodiesToIntegrate_, this->startIndexAndSize_, ephemerisUpdateOrder_, integrationToEphemerisFrameFunctions_ );
    }

private:

    //! List of bodies used in simulations.
    simulation_setup::NamedBodyMap bodyMap_;

    //! List of bodies for which the translational state is numerically integrated.
    /*!
     * List of bodies for which the translational state is numerically integrated. Order in this
     * vector is the same as the order in state vector.
     */
    std::vector< std::string > bodiesToIntegrate_;

    //! Order in which to update the ephemeris objects
    std::vector< std::string > ephemerisUpdateOrder_;

    //! Function to provide the states of the ephemeris origins of each body w.r.t. their respective
    //! integration origins.
    std::map< std::string, boost::function< Eigen::Matrix< StateScalarType, 6, 1 >( const TimeType ) > >
    integrationToEphemerisFrameFunctions_;
};

//! Class used for processing numerically integrated rotational states
/*!
 *  Class used for processing numerically integrated rotational states, updates rotational ephemeris object
 *  of each integrated body.
 */
template< typename TimeType, typename StateScalarType >
class RotationalStateIntegratedStateProcessor: public IntegratedStateProcessor< TimeType, StateScalarType >
{
public:

    //! Constructor.
    /*!
     * Constructor.
     * \param startIndex Index in the state vector where the rotational state starts.
     * \param bodyMap List of bodies used in simulations.
     * \param bodiesToIntegrate List of bodies for which the rotational state is numerically
     * integrated. Order in this vector is the same as the order in state vector.
     */
    RotationalStateIntegratedStateProcessor(
            const int startIndex,
            const simulation_setup::NamedBodyMap& bodyMap,
            const std::vector< std::string >& bodiesToIntegrate ):
        IntegratedStateProcessor< TimeType, StateScalarType >( rotational_state, std::make_pair( startIndex, 7 * bodiesToIntegrate.size( ) ) ),
        bodyMap_( bodyMap ), bodiesToIntegrate_( bodiesToIntegrate )
    { }

    //! Function processing rotational state in the full numericalSolution
    /*!
     * Function that processes the entries of the rotational state in the full numericalSolution,
     * extracts the states for each body, and updates the associated rotational ephemerides.
     * \param numericalSolution Full numerical solution, in global representation (see
     * convertToOutputSolution function in RotationalMotionStateDerivative class.
     */
    void processIntegratedStates(
            const std::map< TimeType, Eigen::Matrix< StateScalarType, Eigen::Dynamic, 1 > >& numericalSolution )
    {
        resetIntegratedRotationalEphemerides< TimeType, StateScalarType >(
                    bodyMap_, numericalSolution, bodiesToIntegrate_ );
    }

    void processIntegratedMultiArcStates(
            const std::vector< std::map< TimeType, Eigen::Matrix< StateScalarType, Eigen::Dynamic, 1 > > >& numericalSolution,
            const std::vector< double >& arcStartTimes )
    {
        throw std::runtime_error( "Error, cannot yet set multi-arc rotational ephemeris" );
    }

private:

    //! List of bodies used in simulations.
    simulation_setup::NamedBodyMap bodyMap_;

    //! List of bodies for which the rotational state is numerically integrated.
    /*!
     * List of bodies for which the rotational state is numerically integrated. Order in this
     * vector is the same as the order in state vector.
     */
    std::vector< std::string > bodiesToIntegrate_;

};

//! Class used for processing numerically integrated masses of bodies.
template< typename TimeType, typename StateScalarType >
class BodyMassIntegratedStateProcessor: public IntegratedStateProcessor< TimeType, StateScalarType >
{
public:

    //! Constructor
    /*!
     * Constructor
     * \param startIndex Index in the state vector where the translational state starts.
     * \param bodyMap List of bodies used in simulations.
     * \param bodiesToIntegrate List of bodies for which the mass is numerically
     * integrated. Order in this vector is the same as the order in state vector.
     */
    BodyMassIntegratedStateProcessor(
            const int startIndex,
            const simulation_setup::NamedBodyMap& bodyMap,
            const std::vector< std::string >& bodiesToIntegrate ):
        IntegratedStateProcessor<  TimeType, StateScalarType >(
            body_mass_state, std::make_pair( startIndex, bodiesToIntegrate.size( ) ) ),
        bodyMap_( bodyMap ), bodiesToIntegrate_( bodiesToIntegrate )
    { }

    //! Destructor
    ~BodyMassIntegratedStateProcessor( ){ }

    //! Function processing mass state in the full numericalSolution
    /*!
     * Function that processes the entries of the propagated mass in the full numericalSolution.
     * \param numericalSolution Full numerical solution of state, in global representation (representation is constant
     * for mass).
     */
    void processIntegratedStates(
            const std::map< TimeType, Eigen::Matrix< StateScalarType, Eigen::Dynamic, 1 > >& numericalSolution )
    {
        resetIntegratedBodyMass( bodyMap_, numericalSolution, bodiesToIntegrate_, this->startIndexAndSize_ );
    }

    void processIntegratedMultiArcStates(
            const std::vector< std::map< TimeType, Eigen::Matrix< StateScalarType, Eigen::Dynamic, 1 > > >& numericalSolution,
            const std::vector< double >& arcStartTimes )
    {
        throw std::runtime_error( "Error, cannot yet reset multi-arc mass model" );
    }

private:

    //! List of bodies used in simulations.
    simulation_setup::NamedBodyMap bodyMap_;

    //! List of bodies for which the body mass is numerically integrated.
    /*!
     * List of bodies for which the body mass is numerically integrated. Order in this
     * vector is the same as the order in state vector.
     */
    std::vector< std::string > bodiesToIntegrate_;
};


//! Function checking feasibility of resetting the translational dynamics
/*!
 * Function to check the feasibility of resetting the translational dynamics of a set of
 * bodies. Function throws error if not feasible.
 * \param bodiesToIntegrate List of bodies to integrate.
 * \param bodyMap List of bodies used in simulations.
 */
void checkTranslationalStatesFeasibility(
        const std::vector< std::string >& bodiesToIntegrate,
        const simulation_setup::NamedBodyMap& bodyMap );


template< typename TimeType, typename StateScalarType >
//! Function to create list objects for processing numerically integrated results.
/*!
 * Function to create list objects for processing numerically integrated results, so that all
 * results are set in the environment models (i.e. resetting tabulated ephemeris for translational
 * dynamics).
 * \param propagatorSettings Settings for the propagation that is used
 * \param bodyMap List of body objects that represents the environemnt
 * \param frameManager Object for providinf conversion functions between different ephemeris origins.
 * \param startIndex Index of state vector where the state entries handled with propagatorSettings
 *        starts.
 * \return List objects for processing numerically integrated results.
 */
std::map< IntegratedStateType,
std::vector< boost::shared_ptr< IntegratedStateProcessor< TimeType, StateScalarType > > > >
createIntegratedStateProcessors(
        const boost::shared_ptr< SingleArcPropagatorSettings< StateScalarType > > propagatorSettings,
        const simulation_setup::NamedBodyMap& bodyMap,
        const boost::shared_ptr< ephemerides::ReferenceFrameManager > frameManager,
        const int startIndex = 0 )
{
    std::map< IntegratedStateType, std::vector< boost::shared_ptr< IntegratedStateProcessor
            < TimeType, StateScalarType > > > > integratedStateProcessors;

    // Check dynamics type.
    switch( propagatorSettings->getStateType( ) )
    {
    case hybrid:
    {
        boost::shared_ptr< MultiTypePropagatorSettings< StateScalarType > > multiTypePropagatorSettings =
                boost::dynamic_pointer_cast< MultiTypePropagatorSettings< StateScalarType > >( propagatorSettings );
        std::map< IntegratedStateType, std::vector< boost::shared_ptr< IntegratedStateProcessor< TimeType, StateScalarType > > > >
                singleTypeIntegratedStateProcessors;
        int currentStartIndex = 0;
        for( typename std::map< IntegratedStateType, std::vector< boost::shared_ptr< SingleArcPropagatorSettings< StateScalarType > > > >::const_iterator
             typeIterator = multiTypePropagatorSettings->propagatorSettingsMap_.begin( );
             typeIterator != multiTypePropagatorSettings->propagatorSettingsMap_.end( ); typeIterator++ )
        {
            if( typeIterator->first != hybrid )
            {
                for( unsigned int i = 0; i < typeIterator->second.size( ); i++ )
                {
                    if(  typeIterator->second.at( i ) == NULL )
                    {
                        std::string errorMessage = "Error in when processing hybrid propagator settings, propagator entry " +
                                boost::lexical_cast< std::string >( i ) + " is not defined.";
                        throw std::runtime_error( errorMessage );
                    }

                    singleTypeIntegratedStateProcessors = createIntegratedStateProcessors< TimeType, StateScalarType >(
                                typeIterator->second.at( i ), bodyMap, frameManager, currentStartIndex );

                    if( singleTypeIntegratedStateProcessors.size( ) > 1 )
                    {
                        throw std::runtime_error( "Error when making hybrid integrated result processors, multiple types found" );
                    }
                    else if( ( singleTypeIntegratedStateProcessors.size( ) == 0 ) &&  ( typeIterator->first != custom_state ) )
                    {
                        throw std::runtime_error( "Error when making hybrid integrated result processors, no types found" );
                    }
                    else if( ( singleTypeIntegratedStateProcessors.size( ) == 1 ) )
                    {
                        if( singleTypeIntegratedStateProcessors.begin( )->second.size( ) != 1 )
                        {
                            throw std::runtime_error( "Error when making hybrid integrated result processors, multiple processors of single type found" );
                        }
                        else
                        {
                            integratedStateProcessors[ singleTypeIntegratedStateProcessors.begin( )->first ].push_back(
                                        singleTypeIntegratedStateProcessors.begin( )->second.at( 0 ) );
                        }
                    }

                    currentStartIndex += typeIterator->second.at( i )->getStateSize( );

                }
            }
            else
            {
                throw std::runtime_error( "Error when making integrated state processors, cannot handle hybrid propagator inside hybrid propagator" );
            }

        }

        break;
    }
    case transational_state:
    {

        // Check input feasibility
        boost::shared_ptr< TranslationalStatePropagatorSettings< StateScalarType > >
                translationalPropagatorSettings = boost::dynamic_pointer_cast
                < TranslationalStatePropagatorSettings< StateScalarType > >( propagatorSettings );
        if( translationalPropagatorSettings == NULL )
        {
            throw std::runtime_error( "Error, input type is inconsistent in createIntegratedStateProcessors" );
        }
        checkTranslationalStatesFeasibility(
                    translationalPropagatorSettings->bodiesToIntegrate_, bodyMap );

        // Create state processors
        integratedStateProcessors[ transational_state ].push_back(
                    boost::make_shared< TranslationalStateIntegratedStateProcessor< TimeType, StateScalarType > >(
                        startIndex, bodyMap, translationalPropagatorSettings->bodiesToIntegrate_,
                        translationalPropagatorSettings->centralBodies_, frameManager ) );
        break;
    }
    case rotational_state:
    {
        boost::shared_ptr< RotationalStatePropagatorSettings< StateScalarType > > rotationalPropagatorSettings =
                boost::dynamic_pointer_cast< RotationalStatePropagatorSettings< StateScalarType > >( propagatorSettings );

        integratedStateProcessors[ rotational_state ].push_back(
                    boost::make_shared< RotationalStateIntegratedStateProcessor< TimeType, StateScalarType > >(
                        startIndex, bodyMap, rotationalPropagatorSettings->bodiesToIntegrate_ ) );
        break;
    }
    case body_mass_state:
    {

        // Check input feasibility
        boost::shared_ptr< MassPropagatorSettings< StateScalarType > >
                massPropagatorSettings = boost::dynamic_pointer_cast
                < MassPropagatorSettings< StateScalarType > >( propagatorSettings );
        if( massPropagatorSettings == NULL )
        {
            throw std::runtime_error( "Error, input type is inconsistent in createIntegratedStateProcessors" );
        }

        // Create mass processors.
        integratedStateProcessors[ body_mass_state ].push_back(
                    boost::make_shared< BodyMassIntegratedStateProcessor< TimeType, StateScalarType > >(
                        startIndex, bodyMap, massPropagatorSettings->bodiesWithMassToPropagate_ ) );
        break;
    }
    case custom_state:
    {
        break;
    }
    default:
        throw std::runtime_error( "Error, could not process integrated state type " +
                                  boost::lexical_cast< std::string >( propagatorSettings->getStateType( ) ) );
    }

    return integratedStateProcessors;
}

//! Function resetting dynamical properties of environment from numerical dynamics solution
/*!
 * Function to reset the dynamical properties of the environment from the numerically integrated
 * dynamics solution
 * \param equationsOfMotionNumericalSolution Solution produced by the numerical integration, in the
 * 'conventional form'
 * \sa SingleStateTypeDerivative::convertToOutputSolution
 * \param integratedStateProcessors List of objects (per dynamics type) used to process integrated
 * results into environment
 */
template< typename TimeType, typename StateScalarType >
void resetIntegratedStates(
        const std::map< TimeType, Eigen::Matrix< StateScalarType, Eigen::Dynamic, 1 > >&
        equationsOfMotionNumericalSolution,
        const std::map< IntegratedStateType,
        std::vector< boost::shared_ptr
        < IntegratedStateProcessor< TimeType, StateScalarType > > > >
        integratedStateProcessors )
{
    for( typename std::map< IntegratedStateType, std::vector< boost::shared_ptr
         < IntegratedStateProcessor< TimeType, StateScalarType > > > >::
         const_iterator updateIterator = integratedStateProcessors.begin( );
         updateIterator != integratedStateProcessors.end( ); updateIterator++ )
    {
        for( unsigned int i = 0; i < updateIterator->second.size( ); i++ )
        {
            updateIterator->second.at( i )->processIntegratedStates(
                        equationsOfMotionNumericalSolution );
        }
    }
}

template< typename TimeType, typename StateScalarType >
void resetIntegratedMultiArcStatesWithEqualArcDynamics(
        const std::vector< std::map< TimeType, Eigen::Matrix< StateScalarType, Eigen::Dynamic, 1 > > >& equationsOfMotionNumericalSolution,
        const std::map< IntegratedStateType, std::vector< boost::shared_ptr< IntegratedStateProcessor< TimeType, StateScalarType > > > >
        integratedStateProcessors,
        const std::vector< double >& arcStartTimes )
{
    for( typename std::map< IntegratedStateType, std::vector< boost::shared_ptr< IntegratedStateProcessor< TimeType, StateScalarType > > > >::
         const_iterator updateIterator = integratedStateProcessors.begin( ); updateIterator != integratedStateProcessors.end( ); updateIterator++ )
    {
        for( unsigned int i = 0; i < updateIterator->second.size( ); i++ )
        {
            updateIterator->second.at( i )->processIntegratedMultiArcStates( equationsOfMotionNumericalSolution, arcStartTimes );
        }
    }
}

} // namespace propagators

} // namespace tudat

#endif // TUDAT_SETNUMERICALLYINTEGRATEDSTATES_H
