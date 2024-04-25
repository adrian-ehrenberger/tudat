// /*    Copyright (c) 2010-2019, Delft University of Technology
//  *    All rigths reserved
//  *
//  *    This file is part of the Tudat. Redistribution and use in source and
//  *    binary forms, with or without modification, are permitted exclusively
//  *    under the terms of the Modified BSD license. You should have received
//  *    a copy of the license with this file. If not, please or visit:
//  *    http://tudat.tudelft.nl/LICENSE.
//  *
//  */

// #include <tudat/astro/aerodynamics/hypersonicFlowAerodynamicCoefficientInterface.h>
// #include <tudat/astro/aerodynamics/hypersonicFlowInteractionModel.h>
// #include <tudat/astro/system_models/vehicleExteriorPanels.h>
// #include <tudat/astro/system_models/vehicleSystems.h>

// // #include "tudat/astro/ephemerides/rotationalEphemeris.h"



// #include <Eigen/Core>
// #include <iostream>
// #include <iostream>
// #include <fstream>
// #include <vector>


// int main( )
// {
    
//     // print hello world
//     std::cout << "Hello World!" << std::endl;


//     tudat::aerodynamics::HypersonicFlowInteractionModel InteractionModel;    
   
//     std::string mesh_data_path = "/tudat-bundle/zz-test/mesh_data";
//     std::vector<double> mesh_panel_normals;
//     std::vector<double> mesh_panel_position_vectors;
//     std::vector<double> mesh_panel_surface_areas;


//     // create vehicle with exterior panels
//     std::ifstream mesh_panel_normals_input(mesh_data_path + "/freePanelNormals.dat");
//     double dval;
//     while (mesh_panel_normals_input >> dval) {
//         mesh_panel_normals.push_back(dval);
//     }
//     mesh_panel_normals_input.close();

//     std::ifstream mesh_panel_position_vectors_input(mesh_data_path + "/freePanelCentroids.dat");
//     while (mesh_panel_position_vectors_input >> dval) {
//         mesh_panel_position_vectors.push_back(dval);
//     }
//     mesh_panel_position_vectors_input.close();

//     std::ifstream mesh_panel_surface_areas_input(mesh_data_path + "/freePanelAreas.dat");
//     while (mesh_panel_surface_areas_input >> dval) {
//         mesh_panel_surface_areas.push_back(dval);
//     }
//     mesh_panel_surface_areas_input.close();


//     // initialize vehicleExteriorPanels
//     std::map< std::string, std::vector< std::shared_ptr< tudat::system_models::VehicleExteriorPanel > > > vehicleExteriorPanels;

//     // initialize single oart of vehicle
//     std::vector< std::shared_ptr< tudat::system_models::VehicleExteriorPanel > > panels;

//     for (int i_panel = 0; i_panel < mesh_panel_surface_areas.size(); i_panel ++) {

//         Eigen::Vector3d normal_vector(mesh_panel_normals[i_panel * 3], mesh_panel_normals[i_panel * 3 + 1], mesh_panel_normals[i_panel * 3 + 2]);

//         Eigen::Vector3d position_vector(mesh_panel_position_vectors[i_panel * 3], mesh_panel_position_vectors[i_panel * 3 + 1], mesh_panel_position_vectors[i_panel * 3 + 2]);

//         // Create a shared pointer using std::make_shared
//         auto panel = std::make_shared<tudat::system_models::VehicleExteriorPanel>(
//             normal_vector,
//             position_vector,
//             mesh_panel_surface_areas[i_panel]  // Assuming each panel corresponds to three values in the vectors
//         );

//         // Add the shared pointer to the vector
//         panels.push_back(panel);

//     }
    
//     vehicleExteriorPanels["vehicle"] = panels;

//     for (auto it = vehicleExteriorPanels.begin(); it != vehicleExteriorPanels.end(); ++it) {
//         const std::string& vehiclePartName = it->first;

//         std::cout << "Vehicle part name: " << vehiclePartName << std::endl;
//     }

//     // // initialize vehiclePartOrientation
//     // std::map< std::string, std::shared_ptr< tudat::ephemerides::RotationalEphemeris > > vehiclePartOrientation;
//     // vehiclePartOrientation["vehicle"] = std::make_shared< tudat::ephemerides::RotationalEphemeris >();


//     tudat::system_models::VehicleSystems vehicle = tudat::system_models::VehicleSystems( );

//     vehicle.setVehicleExteriorPanels(vehicleExteriorPanels);


//     tudat::aerodynamics::HypersonicFlowAerodynamicCoefficientInterface coeff_interface(
//         vehicle,
//         1.0, 1.0, Eigen::Vector3d(0.0, 0.0, 0.0)
//     );

//     // print coefficients 

//     std::cout << "Coefficients: " << coeff_interface.getCurrentAerodynamicCoefficients() << std::endl;

//     // upadte coefficients

//     std::vector<double> independentVariables = {0.0, 0.0};
//     //                                          aoa side  
//     coeff_interface.updateCurrentCoefficients(independentVariables, 0.0);

//     std::cout << "Coefficients: " << coeff_interface.getCurrentAerodynamicCoefficients() << std::endl;

//     // print goodbye
//     std::cout << "Goodbye World!" << std::endl;


//     return 0;

// }