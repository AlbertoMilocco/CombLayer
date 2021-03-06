/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   moderator/makeTS2.cxx
*
 * Copyright (c) 2004-2013 by Stuart Ansell
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>. 
 *
 ****************************************************************************/
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <cmath>
#include <complex>
#include <list>
#include <vector>
#include <set>
#include <map>
#include <string>
#include <algorithm>
#include <iterator>
#include <boost/array.hpp>
#include <boost/format.hpp>
#include <boost/shared_ptr.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "inputParam.h"
#include "Triple.h"
#include "NRange.h"
#include "NList.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "Rules.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "KGroup.h"
#include "Source.h"
#include "KCode.h"
#include "insertInfo.h"
#include "insertBaseInfo.h"
#include "InsertComp.h"
#include "ModeCard.h"
#include "PhysImp.h"
#include "PhysCard.h"
#include "LSwitchCard.h"
#include "PhysicsCards.h"
#include "Simulation.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "LayerComp.h"
#include "SecondTrack.h"
#include "TwinComp.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "LinearComp.h"
#include "AttachSupport.h"
#include "FlightLine.h"
#include "pipeUnit.h"
#include "PipeLine.h"

#include "shutterBlock.h"
#include "GeneralShutter.h"
#include "BulkInsert.h"
#include "ChipIRFilter.h"
#include "ChipIRGuide.h"
#include "ChipIRInsert.h"
#include "ChipIRSource.h"
#include "InnerWall.h"
#include "HoleUnit.h"
#include "PreCollimator.h"
#include "Collimator.h"
#include "ColBox.h"
#include "beamBlock.h"
#include "BeamStop.h"
#include "Table.h"
#include "Hutch.h"
#include "collInsertBase.h"
#include "collInsertBlock.h"
#include "PointWeights.h"
#include "FBBlock.h"
#include "makeChipIR.h"
#include "makeZoom.h"

#include "VoidVessel.h"
#include "BulkShield.h"
#include "Groove.h"
#include "Hydrogen.h"
#include "VacVessel.h"
#include "FlightLine.h"
#include "PreMod.h"
#include "HWrapper.h"
#include "Decoupled.h"
#include "RefCutOut.h"
#include "Bucket.h"
#include "CoolPad.h"
#include "Reflector.h"
#include "makeIMat.h"
#include "makeTS2Bulk.h"

#include "makeTS2.h"

namespace moderatorSystem
{

makeTS2::makeTS2()
/*!
  Constructor
 */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();
}

makeTS2::~makeTS2()
/*!
  Destructor
 */
{}

void 
makeTS2::build(Simulation* SimPtr,
		  const mainSystem::inputParam& IParam)
  
  /*!
    Carry out the full build
    \param SimPtr :: Simulation system
    \param IParam :: Input parameters
   */
{
  // For output stream
  ELog::RegMethod RControl("makeTS2","build");


  moderatorSystem::makeTS2Bulk bulkObj;
  hutchSystem::makeChipIR chipObj;
  zoomSystem::makeZoom zoomObj;
  imatSystem::makeIMat imatObj;
  
  bulkObj.build(SimPtr,IParam);
  if (!IParam.flag("exclude") ||
      (!IParam.compValue("E",std::string("Bulk"))) ) 
    {
      chipObj.build(SimPtr,IParam,*bulkObj.getBulkShield());
      zoomObj.build(SimPtr,IParam,*bulkObj.getBulkShield());
      imatObj.build(SimPtr,IParam,*bulkObj.getBulkShield());
    }
  // Insert pipes [make part of makeTS2Bulk]
  bulkObj.insertPipeObjects(SimPtr,IParam);

  return;
}

}   // NAMESPACE ts1System

