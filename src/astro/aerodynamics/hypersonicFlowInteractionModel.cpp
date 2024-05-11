/*    Copyright (c) 2010-2022, Delft University of Technology
 *    All rigths reserved
 *
 *    This file is part of the Tudat. Redistribution and use in source and
 *    binary forms, with or without modification, are permitted exclusively
 *    under the terms of the Modified BSD license. You should have received
 *    a copy of the license with this file. If not, please or visit:
 *    http://tudat.tudelft.nl/LICENSE.
 */

#include "tudat/astro/aerodynamics/hypersonicFlowInteractionModel.h"

#include <Eigen/Core>
#include <iostream>
#include "tudat/math/basic/linearAlgebra.h"
#include "tudat/math/basic/mathematicalConstants.h"

namespace tudat
{
namespace aerodynamics
{

// RarefiedFlowInteractionModel::RarefiedFlowInteractionModel() = default;

/*! Computes aerodynamic force coefficients for a single panel
*  \param inclinationAngle Cosine of the angle between the freestream velocity and the panel normal
*  \param cosineOfNormalLiftAngle Cosine of the angle between the freestream velocity and the panel normal
*  \param panelSurfaceArea Area of the panel
*  \param panelTemperature Temperature of the panel
*  \param liftUnitVector Lift vector
*  \param dragUnitVector Drag vector
*  \param freestreamVelocity Freestream velocity
*  \param atmosphericTemperature Atmospheric temperature
*  \param numberDensities Number densities of species
*  \param totalNumberDensity Total number density
*  \param referenceArea Reference area
*  \return Force coefficient vector
*/
double HypersonicFlowInteractionModel::computePanelPressureCoefficient( 
    double inclinationAngle
    )
{

    double panelPressureCoefficient = 0.0;

    std::cout << inclinationAngle << std::endl;

    // if your're reading this, I apologize for the shit code. Just trying to get it to work atm....
    if (inclinationAngle <= -3.14159265359/2 || inclinationAngle >= 3.14159265359/2)
    {
        panelPressureCoefficient = 0.0;

    }else{

        // double panelPressureCoefficient_ = computeNewtonianPressureCoefficient( inclinationAngle );

        // for testing purposes        
        panelPressureCoefficient = 2.0 * pow( cos( inclinationAngle ), 2.0 );
    }

    return panelPressureCoefficient;
}


Eigen::Vector3d HypersonicFlowInteractionModel::computePanelForceCoefficientVector(
    double panelArea,
    double referenceArea,
    double panelCosineLiftAngle,
    double panelCosineDragAngle,
    Eigen::Vector3d liftUnitVector,
    Eigen::Vector3d dragUnitVetor
    )
{
    

    double inclinationAngle = acos( panelCosineDragAngle );


    double panelPressureCoefficient = computePanelPressureCoefficient( inclinationAngle );


    Eigen::Vector3d panelForceCoefficientVector = panelPressureCoefficient * panelCosineLiftAngle * liftUnitVector + panelPressureCoefficient * panelCosineDragAngle * dragUnitVetor;

    // Normalize result by reference area.
    panelForceCoefficientVector = panelForceCoefficientVector * panelArea / referenceArea;

    return panelForceCoefficientVector;

}

Eigen::Vector3d HypersonicFlowInteractionModel::computePanelMomentCoefficientVector( 
    Eigen::Vector3d panelForceCoefficientVector,
    Eigen::Vector3d panelPositionVector,
    double referenceLength)
{
    return panelPositionVector.cross(panelForceCoefficientVector) / referenceLength;
}



} // tudat
} // electromagnetism
