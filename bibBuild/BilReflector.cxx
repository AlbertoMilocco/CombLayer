/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   bibBuild/BilReflector.cxx
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
#include "TargetBase.h"
#include "TS2target.h"
#include "TS2moly.h"
#include "Groove.h"
#include "Hydrogen.h"
#include "OrthoInsert.h"
#include "VacVessel.h"
#include "FlightLine.h"
#include "World.h"
#include "BilReflector.h"

namespace bibSystem
{

BilReflector::BilReflector(const std::string& Key)  :
  attachSystem::ContainedComp(),attachSystem::FixedComp(Key,3),
  refIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(refIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

BilReflector::BilReflector(const BilReflector& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedComp(A),
  refIndex(A.refIndex),cellIndex(A.cellIndex),xStep(A.xStep),
  yStep(A.yStep),zStep(A.zStep),xyAngle(A.xyAngle),
  zAngle(A.zAngle),BeHeight(A.BeHeight),BeDepth(A.BeDepth),
  BeRadius(A.BeRadius),BeMat(A.BeMat),InnerHeight(A.InnerHeight),
  InnerDepth(A.InnerDepth),InnerRadius(A.InnerRadius),
  InnerMat(A.InnerMat),PbHeight(A.PbHeight),PbDepth(A.PbDepth),
  PbRadius(A.PbRadius),PbMat(A.PbMat),MidHeight(A.MidHeight),
  MidDepth(A.MidDepth),MidRadius(A.MidRadius),MidMat(A.MidMat),
  OuterHeight(A.OuterHeight),OuterDepth(A.OuterDepth),
  OuterRadius(A.OuterRadius),OuterMat(A.OuterMat),
  OuterPbHeight(A.OuterPbHeight),OuterPbDepth(A.OuterPbDepth),
  OuterPbRadius(A.OuterPbRadius),OuterPbMat(A.OuterPbMat)
  /*!
    Copy constructor
    \param A :: BilReflector to copy
  */
{}

BilReflector&
BilReflector::operator=(const BilReflector& A)
  /*!
    Assignment operator
    \param A :: BilReflector to copy
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
      BeHeight=A.BeHeight;
      BeDepth=A.BeDepth;
      BeRadius=A.BeRadius;
      BeMat=A.BeMat;
      InnerHeight=A.InnerHeight;
      InnerDepth=A.InnerDepth;
      InnerRadius=A.InnerRadius;
      InnerMat=A.InnerMat;
      PbHeight=A.PbHeight;
      PbDepth=A.PbDepth;
      PbRadius=A.PbRadius;
      PbMat=A.PbMat;
      MidHeight=A.MidHeight;
      MidDepth=A.MidDepth;
      MidRadius=A.MidRadius;
      MidMat=A.MidMat;
      OuterHeight=A.OuterHeight;
      OuterDepth=A.OuterDepth;
      OuterRadius=A.OuterRadius;
      OuterMat=A.OuterMat;
      OuterPbHeight=A.OuterPbHeight;
      OuterPbDepth=A.OuterPbDepth;
      OuterPbRadius=A.OuterPbRadius;
      OuterPbMat=A.OuterPbMat;
    }
  return *this;
}

BilReflector::~BilReflector() 
  /*!
    Destructor
  */
{}


void
BilReflector::populate(const Simulation& System)
 /*!
   Populate all the variables
   \param System :: Simulation to use
 */
{
  ELog::RegMethod RegA("BilReflector","populate");
  
  const FuncDataBase& Control=System.getDataBase();
  
  xStep=Control.EvalVar<double>(keyName+"XStep");
  yStep=Control.EvalVar<double>(keyName+"YStep");
  zStep=Control.EvalVar<double>(keyName+"ZStep");
  xyAngle=Control.EvalVar<double>(keyName+"XYAngle");
  zAngle=Control.EvalVar<double>(keyName+"ZAngle");

  BeHeight=Control.EvalVar<double>(keyName+"BeHeight");
  BeDepth=Control.EvalVar<double>(keyName+"BeDepth");
  BeRadius=Control.EvalVar<double>(keyName+"BeRadius");
  BeMat=Control.EvalVar<int>(keyName+"BeMat");

  InnerHeight=Control.EvalVar<double>(keyName+"InnerHeight");
  InnerDepth=Control.EvalVar<double>(keyName+"InnerDepth");
  InnerRadius=Control.EvalVar<double>(keyName+"InnerRadius");
  InnerMat=Control.EvalVar<int>(keyName+"InnerMat");

  PbHeight=Control.EvalVar<double>(keyName+"PbHeight");
  PbDepth=Control.EvalVar<double>(keyName+"PbDepth");
  PbRadius=Control.EvalVar<double>(keyName+"PbRadius");
  PbMat=Control.EvalVar<int>(keyName+"PbMat");

  MidHeight=Control.EvalVar<double>(keyName+"MidHeight");
  MidDepth=Control.EvalVar<double>(keyName+"MidDepth");
  MidRadius=Control.EvalVar<double>(keyName+"MidRadius");
  MidMat=Control.EvalVar<int>(keyName+"MidMat");

  OuterHeight=Control.EvalVar<double>(keyName+"OuterHeight");
  OuterDepth=Control.EvalVar<double>(keyName+"OuterDepth");
  OuterRadius=Control.EvalVar<double>(keyName+"OuterRadius");
  OuterMat=Control.EvalVar<int>(keyName+"OuterMat");

  OuterPbHeight=Control.EvalVar<double>(keyName+"OuterPbHeight");
  OuterPbDepth=Control.EvalVar<double>(keyName+"OuterPbDepth");
  OuterPbRadius=Control.EvalVar<double>(keyName+"OuterPbRadius");
  OuterPbMat=Control.EvalVar<int>(keyName+"OuterPbMat");

  return;
}
  
void
BilReflector::createUnitVector(const attachSystem::FixedComp& FC,
			       const size_t sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Linked object
    \param sideIndex :: Origin link point 
  */
{
  ELog::RegMethod RegA("BilReflector","createUnitVector");
  attachSystem::FixedComp::createUnitVector(FC);
  Origin=FC.getLinkPt(sideIndex); /** Aqui pide el punto*/

  // ANGLE SHIFT [Do not remove! -- the reflect is never
  // optimally at the centre of the target
  applyShift(xStep,yStep,zStep);
  applyAngleRotate(xyAngle,zAngle);

  return;
}
  
void
BilReflector::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("BilReflector","createSurface");

  // Superficies del reflector de berilio

  const double H[]={BeHeight,InnerHeight,PbHeight,MidHeight,
	      OuterHeight,OuterPbHeight};

  const double D[]={BeDepth,InnerDepth,PbDepth,MidDepth,
	      OuterDepth,OuterPbDepth};

  const double R[]={BeRadius,InnerRadius,PbRadius,MidRadius,
	      OuterRadius,OuterPbRadius};

  const size_t nLayer(sizeof(R)/sizeof(double));
  int RI(refIndex);
  for(size_t i=0;i<nLayer;i++)
    {
      ModelSupport::buildPlane(SMap,RI+5,Origin-Z*D[i],Z);
      ModelSupport::buildPlane(SMap,RI+6,Origin+Z*H[i],Z);
      ModelSupport::buildCylinder(SMap,RI+7,Origin,Z,R[i]);
      RI+=10;
    }
  return;
}

void 
BilReflector::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("BilReflector","createObjects");

  std::string Out;

  const int M[]={BeMat,InnerMat,PbMat,MidMat,
		 OuterMat,OuterPbMat};
  const size_t nLayer(sizeof(M)/sizeof(int));
  int RI(refIndex);
  for(size_t i=0;i<nLayer;i++)
    {
      Out=ModelSupport::getComposite(SMap,RI,"5 -6 -7");
      if (i)
	Out+=ModelSupport::getComposite(SMap,RI-10,"(-5:6:7)");
      System.addCell(MonteCarlo::Qhull(cellIndex++,M[i],0.0,Out));
      RI+=10;
    }

  Out=ModelSupport::getComposite(SMap,RI-10,"5 -6 -7");

  addOuterSurf(Out);
  addBoundarySurf(Out);
  return;
}

void 
BilReflector::createLinks()
  /*!
    Create all the linkes [OutGoing]
  */
{
  ELog::RegMethod RegA("Reflector","createLinks");

  const double H(BeHeight+InnerHeight+PbHeight+MidHeight+
		 OuterHeight+OuterPbHeight);
  const double D(BeDepth+InnerDepth+PbDepth+MidDepth+
		 OuterDepth+OuterPbDepth);
  const double R(BeRadius+InnerRadius+PbRadius+MidRadius+
		    OuterRadius+OuterPbRadius);

  FixedComp::setConnect(0,Origin-Z*D,-Z);
  FixedComp::setLinkSurf(0,SMap.realSurf(refIndex+55));

  FixedComp::setConnect(1,Origin+Z*H,Z);
  FixedComp::setLinkSurf(1,-SMap.realSurf(refIndex+56));

  FixedComp::setConnect(2,Origin+Y*R,Y);
  FixedComp::setLinkSurf(2,SMap.realSurf(refIndex+57));

  return;
}

void
BilReflector::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const size_t sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
  */
{
  ELog::RegMethod RegA("BilReflector","createAll");

  populate(System);
  createUnitVector(FC,sideIndex);

  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // NAMESPACE ts1System
