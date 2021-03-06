/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   essBuild/BulkModule.cxx
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
#include <boost/shared_ptr.hpp>
#include <boost/array.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "stringCombine.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Triple.h"
#include "NRange.h"
#include "NList.h"
#include "Tally.h"
#include "Quaternion.h"
#include "localRotate.h"
#include "masterRotate.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "surfEqual.h"
#include "surfDivide.h"
#include "surfDIter.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Line.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "inputParam.h"
#include "ReadFunctions.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "KGroup.h"
#include "Source.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "generateSurf.h"
#include "chipDataStore.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "World.h"
#include "BulkModule.h"

namespace essSystem
{

BulkModule::BulkModule(const std::string& Key)  :
  attachSystem::ContainedComp(),attachSystem::FixedComp(Key,6),
  bulkIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(bulkIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

BulkModule::BulkModule(const BulkModule& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedComp(A),
  bulkIndex(A.bulkIndex),cellIndex(A.cellIndex),
  xStep(A.xStep),yStep(A.yStep),zStep(A.zStep),
  xyAngle(A.xyAngle),zAngle(A.zAngle),nLayer(A.nLayer),
  radius(A.radius),height(A.height),depth(A.depth),
  COffset(A.COffset),Mat(A.Mat)
  /*!
    Copy constructor
    \param A :: BulkModule to copy
  */
{}

BulkModule&
BulkModule::operator=(const BulkModule& A)
  /*!
    Assignment operator
    \param A :: BulkModule to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedComp::operator=(A);
      cellIndex=A.cellIndex;
      xStep=A.xStep;
      yStep=A.yStep;
      zStep=A.zStep;
      xyAngle=A.xyAngle;
      zAngle=A.zAngle;
      nLayer=A.nLayer;
      radius=A.radius;
      height=A.height;
      depth=A.depth;
      COffset=A.COffset;
      Mat=A.Mat;
    }
  return *this;
}

BulkModule::~BulkModule() 
  /*!
    Destructor
  */
{}

void
BulkModule::populate(const Simulation& System)
 /*!
   Populate all the variables
   \param System :: Simulation to use
 */
{
  ELog::RegMethod RegA("BulkModule","populate");
  
  const FuncDataBase& Control=System.getDataBase();
  
  xStep=Control.EvalVar<double>(keyName+"XStep");
  yStep=Control.EvalVar<double>(keyName+"YStep");
  zStep=Control.EvalVar<double>(keyName+"ZStep");
  xyAngle=Control.EvalVar<double>(keyName+"XYangle");
  zAngle=Control.EvalVar<double>(keyName+"Zangle");

  nLayer=Control.EvalVar<size_t>(keyName+"NLayer");
  if (!nLayer) return;
  radius.resize(nLayer);
  height.resize(nLayer);
  depth.resize(nLayer);
  Mat.resize(nLayer);
  COffset.resize(nLayer);
  for(size_t i=0;i<nLayer;i++)
    {
      radius[i]=Control.EvalVar<double>
	(keyName+StrFunc::makeString("Radius",i+1));
      height[i]=Control.EvalVar<double>
	(keyName+StrFunc::makeString("Height",i+1));
      depth[i]=Control.EvalVar<double>
	(keyName+StrFunc::makeString("Depth",i+1));
      Mat[i]=Control.EvalVar<int>
	(keyName+StrFunc::makeString("Mat",i+1));
      if (i)
	COffset[i]=Control.EvalDefVar<Geometry::Vec3D>
	  (keyName+StrFunc::makeString("Offset",i+1),
	   Geometry::Vec3D(0,0,0));
    }
  return;
}
  
void
BulkModule::createUnitVector(const attachSystem::FixedComp& FC)
  /*!
    Create the unit vectors
    \param FC :: Linked object
  */
{
  ELog::RegMethod RegA("BulkModule","createUnitVector");

  FixedComp::createUnitVector(FC);
  applyShift(xStep,yStep,zStep);
  applyAngleRotate(xyAngle,zAngle);
    
  for(size_t i=0;i<nLayer;i++)
    COffset[i]=X*COffset[i].X()+Y*COffset[i].Y();

  return;
}
  
void
BulkModule::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("BulkModule","createSurface");

  // rotation of axis:

  int RI(bulkIndex);
  for(size_t i=0;i<nLayer;i++)
    {
      ModelSupport::buildPlane(SMap,RI+5,Origin-Z*depth[i],Z);
      ModelSupport::buildPlane(SMap,RI+6,Origin+Z*height[i],Z);
      ModelSupport::buildCylinder(SMap,RI+7,Origin+COffset[i]
				  ,Z,radius[i]);
      RI+=10;
    }

  return;
}

void
BulkModule::createObjects(Simulation& System,
			  const attachSystem::ContainedComp& CC)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
    \param CC :: Inner reflector
  */
{
  ELog::RegMethod RegA("BulkModule","createObjects");

  std::string Out,OutX;
  int RI(bulkIndex);
  for(size_t i=0;i<nLayer;i++)
    {
      Out=ModelSupport::getComposite(SMap,RI,"5 -6 -7 ");
      if (i)
	OutX=ModelSupport::getComposite(SMap,RI-10,"(-5:6:7)");
      else
	OutX=CC.getExclude();
      System.addCell(MonteCarlo::Qhull(cellIndex++,Mat[i],0.0,Out+OutX));
      RI+=10;
    }
  
  addOuterSurf(Out);
  return;
}

std::string
BulkModule::getComposite(const std::string& surfList) const
  /*!
    Exposes local version of getComposite
    \param surfList :: surface list
    \return Composite string
  */
{
  return ModelSupport::getComposite(SMap,bulkIndex,surfList);
}

void
BulkModule::createLinks()
  /*!
    Create all the linkes [OutGoing]
  */
{
  ELog::RegMethod RegA("BulkModule","createLinks");

  if (nLayer>1)
    {
      size_t index(0);
      for(size_t j=0;j<2;j++)
        {
	  const size_t i(nLayer-(j+1));
	  
	  FixedComp::setConnect
	    (index,Origin+COffset[i]-Z*depth[i],-Z);  // base
	  FixedComp::setConnect
	    (index+1,Origin+COffset[i]+Z*height[i],Z);  // top
	  FixedComp::setConnect
	    (index+2,Origin+COffset[i]+Y*radius[i],Y);   // outer point
	  
	  const int RI(static_cast<int>(i)*10+bulkIndex);
	  FixedComp::setLinkSurf(index,-SMap.realSurf(RI+5));
	  FixedComp::setLinkSurf(index+1,SMap.realSurf(RI+6));
	  FixedComp::setLinkSurf(index+2,SMap.realSurf(RI+7));
	  index+=3;
	}
    }
  return;
}

void 
BulkModule::addFlightUnit(Simulation& System,
			  const attachSystem::FixedComp& FC)
  /*!
    Adds a flight unit based on an existing flight line
    it is effectively an extension. This method only works
    with a >=2 layer system.
    \param System :: simulation
    \param FC :: Flight component [uses link surf 2-5] 
  */
{
  ELog::RegMethod RegA("BulkModule","addFlightUnit");

  std::string Out;
  std::stringstream cx;
  cx<<" (";
  for(size_t index=2;index<5;index++)
    cx<<FC.getLinkString(index)<<" : ";
  cx<<FC.getLinkString(5)<<" )";

  // AVOID INNER
  for(int i=1;i<static_cast<int>(nLayer);i++)
    {
      MonteCarlo::Qhull* OPtr=System.findQhull(bulkIndex+i+1);
      if (!OPtr)
	throw ColErr::InContainerError<int>(bulkIndex+i+1,"layerCells");
      OPtr->addSurfString(cx.str());
    }
  // Now make internal surface
  cx.str("");
  for(size_t index=2;index<6;index++)
    cx<<FC.getLinkComplement(index)<<" ";

  Out=ModelSupport::getComposite(SMap,bulkIndex,
				 bulkIndex+10*static_cast<int>(nLayer-1),
				 " 7 -7M ");
  // Dividing surface ?
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out+cx.str()));
  return;
}

void
BulkModule::createAll(Simulation& System,
		      const attachSystem::FixedComp& FC,
		      const attachSystem::ContainedComp& CC)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param CC :: Central origin
  */
{
  ELog::RegMethod RegA("BulkModule","createAll");

  populate(System);
  createUnitVector(FC);
  createSurfaces();
  createLinks();
  createObjects(System,CC);
  insertObjects(System);              

  return;
}

}  // NAMESPACE ts1System
