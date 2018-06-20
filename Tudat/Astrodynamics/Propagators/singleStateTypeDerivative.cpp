/*    Copyright (c) 2010-2018, Delft University of Technology
 *    All rigths reserved
 *
 *    This file is part of the Tudat. Redistribution and use in source and
 *    binary forms, with or without modification, are permitted exclusively
 *    under the terms of the Modified BSD license. You should have received
 *    a copy of the license with this file. If not, please or visit:
 *    http://tudat.tudelft.nl/LICENSE.
 */
#include "Tudat/Astrodynamics/Propagators/singleStateTypeDerivative.h"

namespace tudat
{

namespace propagators
{

//! Get size of state for single propagated state of given type.
int getSingleIntegrationSize( const IntegratedStateType stateType )
{
    int singleStateSize = 0;
    switch( stateType )
    {
    case transational_state:
        singleStateSize = 6;
        break;
    case rotational_state:
        singleStateSize = 7;
        break;
    case body_mass_state:
        singleStateSize = 1;
        break;
    default:
        std::string errorMessage =
                "Did not recognize state type " + std::to_string( stateType ) + "when getting size";
       throw std::runtime_error( errorMessage );
    }
    return singleStateSize;
}

//! Get order of differential equation for governing equations of dynamics of given type.
int getSingleIntegrationDifferentialEquationOrder( const IntegratedStateType stateType )
{
    int singleStateSize = 0;
    switch( stateType )
    {
    case transational_state:
        singleStateSize = 2;
        break;
    case body_mass_state:
        singleStateSize = 1;
        break;
    case rotational_state:
        singleStateSize = 1;
        break;
    default:
        std::string errorMessage =
                "Did not recognize state type " + std::to_string( stateType ) + "when getting order";
       throw std::runtime_error( errorMessage );
    }
    return singleStateSize;
}

int getAccelerationSize( const IntegratedStateType stateType )
{
    int accelerationSize = 0;
    switch( stateType )
    {
    case transational_state:
        accelerationSize = 3;
        break;
    case body_mass_state:
        accelerationSize = 1;
        break;
    case rotational_state:
        accelerationSize = 3;
        break;
    default:
        std::string errorMessage =
                "Did not recognize state type " + std::to_string( stateType ) + "when getting acceleration sizw";
       throw std::runtime_error( errorMessage );
    }
    return accelerationSize;
}


template class SingleStateTypeDerivative< double, double >;
template class SingleStateTypeDerivative< long double, double >;
template class SingleStateTypeDerivative< double, Time >;
template class SingleStateTypeDerivative< long double, Time >;

}

}
