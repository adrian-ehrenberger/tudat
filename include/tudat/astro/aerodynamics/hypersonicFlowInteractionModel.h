/*    Copyright (c) 2010-2022, Delft University of Technology
 *    All rigths reserved
 *
 *    This file is part of the Tudat. Redistribution and use in source and
 *    binary forms, with or without modification, are permitted exclusively
 *    under the terms of the Modified BSD license. You should have received
 *    a copy of the license with this file. If not, please or visit:
 *    http://tudat.tudelft.nl/LICENSE.
 *
 */

#ifndef TUDAT_HYPERSONIC_FLOW_INTERACTION_MODEL_H
#define TUDAT_HYPERSONIC_FLOW_INTERACTION_MODEL_H

#include <Eigen/Core>

namespace tudat
{
namespace aerodynamics
{

class HypersonicFlowInteractionModel{
public:
    HypersonicFlowInteractionModel() = default;

    virtual ~HypersonicFlowInteractionModel() = default;

    double computePanelPressureCoefficient(
        // TBD
    );


};
} // tudat
} // aerodynamic

#endif //TUDAT_HYPERSONIC_FLOW_INTERACTION_MODEL_H
