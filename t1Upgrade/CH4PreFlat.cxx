/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   t1Upgrade/CH4PreFlat.cxx
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
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "KGroup.h"
#include "Source.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "generateSurf.h"
#include "surfExpand.h"
#include "chipDataStore.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "ContainedComp.h"
#include "CH4PreModBase.h"
#include "CH4PreFlat.h"

namespace ts1System
{

CH4PreFlat::CH4PreFlat(const std::string& Key)  :
  CH4PreModBase(Key,6)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

CH4PreFlat::CH4PreFlat(const CH4PreFlat& A) : 
  CH4PreModBase(A),
  xStep(A.xStep),yStep(A.yStep),zStep(A.zStep),
  xyAngle(A.xyAngle),zAngle(A.zAngle),width(A.width),
  height(A.height),depth(A.depth),alThick(A.alThick),
  vacThick(A.vacThick),modTemp(A.modTemp),modMat(A.modMat),
  alMat(A.alMat)
  /*!
    Copy constructor
    \param A :: CH4PreFlat to copy
  */
{}

CH4PreFlat&
CH4PreFlat::operator=(const CH4PreFlat& A)
  /*!
    Assignment operator
    \param A :: CH4PreFlat to copy
    \return *this
  */
{
  if (this!=&A)
    {
      CH4PreModBase::operator=(A);
      xStep=A.xStep;
      yStep=A.yStep;
      zStep=A.zStep;
      xyAngle=A.xyAngle;
      zAngle=A.zAngle;
      width=A.width;
      height=A.height;
      depth=A.depth;
      alThick=A.alThick;
      vacThick=A.vacThick;
      modTemp=A.modTemp;
      modMat=A.modMat;
      alMat=A.alMat;
    }
  return *this;
}

CH4PreFlat::~CH4PreFlat() 
  /*!
    Destructor
  */
{}

CH4PreFlat*
CH4PreFlat::clone()  const
  /*!
    Clone method
    \return copy of this
  */
{
  return new CH4PreFlat(*this);
}

void
CH4PreFlat::populate(const Simulation& System)
  /*!
    Populate all the variables
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("CH4PreFlat","populate");
  
  const FuncDataBase& Control=System.getDataBase();
  
  xStep=Control.EvalVar<double>(keyName+"XStep");
  yStep=Control.EvalVar<double>(keyName+"YStep");
  zStep=Control.EvalVar<double>(keyName+"ZStep");
  xyAngle=Control.EvalVar<double>(keyName+"XYangle");
  zAngle=Control.EvalVar<double>(keyName+"Zangle");

  width=Control.EvalVar<double>(keyName+"Width");
  height=Control.EvalVar<double>(keyName+"Height");
  depth=Control.EvalVar<double>(keyName+"Depth");

  alThick=Control.EvalVar<double>(keyName+"AlThick");
  vacThick=Control.EvalVar<double>(keyName+"VacThick");

  modTemp=Control.EvalVar<int>(keyName+"ModTemp");
  modMat=Control.EvalVar<int>(keyName+"ModMat");
  alMat=Control.EvalVar<int>(keyName+"AlMat");
  
  return;
}
  
void
CH4PreFlat::createUnitVector(const attachSystem::FixedComp& FC,
			     const size_t linkPt)
  /*!
    Create the unit vectors
    \param FC :: Component to connect to
    \param linkPt :: Link point to base of moderator
  */
{
  ELog::RegMethod RegA("CH4PreFlat","createUnitVector");

  FixedComp::createUnitVector(FC);
  Origin=FC.getLinkPt(linkPt);
  // Note shift to move it to the centre:
  applyShift(xStep,yStep,zStep+alThick+height/2.0);
  applyAngleRotate(xyAngle,zAngle);

  return;
}
  
void
CH4PreFlat::createSurfaces(const attachSystem::FixedComp& FC,
			   const size_t linkPt)
  /*!
    Create All the surfaces
    \param FC :: Fixed unit that connects to this moderator
    \param linkPt :: Base of the moderator if connected
  */
{
  ELog::RegMethod RegA("CH4PreFlat","createSurface");

  // NOTE Origin is moved from moderator base:
  const int LN=FC.getLinkSurf(linkPt);
  
  ModelSupport::buildPlane(SMap,preIndex+1,Origin-Y*(depth/2.0),Y);
  ModelSupport::buildPlane(SMap,preIndex+2,Origin+Y*(depth/2.0),Y);
  ModelSupport::buildPlane(SMap,preIndex+3,Origin-X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,preIndex+4,Origin+X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,preIndex+5,Origin-Z*(height/2.0),Z);
  ModelSupport::buildPlane(SMap,preIndex+6,Origin+Z*(height/2.0),Z);


  // AL plane contacts
  ModelSupport::buildPlane(SMap,preIndex+11,
			   Origin-Y*(depth/2.0+alThick),Y);
  ModelSupport::buildPlane(SMap,preIndex+12,
			   Origin+Y*(depth/2.0+alThick),Y);
  ModelSupport::buildPlane(SMap,preIndex+13,
			   Origin-X*(width/2.0+alThick),X);
  ModelSupport::buildPlane(SMap,preIndex+14,
			   Origin+X*(width/2.0+alThick),X);

  ModelSupport::buildPlane(SMap,preIndex+15,
			   Origin-Z*(height/2.0+alThick),Z);
  ModelSupport::buildPlane(SMap,preIndex+25,
			   Origin-Z*(height/2.0+alThick+vacThick),Z);
  ModelSupport::buildPlane(SMap,preIndex+16,
			   Origin+Z*(height/2.0+alThick),Z);


  // VAC GAP
  ModelSupport::buildPlane(SMap,preIndex+21,
			   Origin-Y*(depth/2.0+alThick+vacThick),Y);
  ModelSupport::buildPlane(SMap,preIndex+22,
			   Origin+Y*(depth/2.0+alThick+vacThick),Y);
  ModelSupport::buildPlane(SMap,preIndex+23,
			   Origin-X*(width/2.0+alThick+vacThick),X);
  ModelSupport::buildPlane(SMap,preIndex+24,
			   Origin+X*(width/2.0+alThick+vacThick),X);
  ModelSupport::buildPlane(SMap,preIndex+26,
			   Origin+Z*(height/2.0+alThick+vacThick),Z);

  return;
}

void
CH4PreFlat::createObjects(Simulation& System)
  /*!
    Adds the Chip guide components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("CH4PreFlat","createObjects");
  std::string Out;

  Out=ModelSupport::getSetComposite(SMap,preIndex,"1 -2 3 -4 5 -6");
  System.addCell(MonteCarlo::Qhull(cellIndex++,modMat,modTemp,Out));
  // AL layer
  Out=ModelSupport::getSetComposite(SMap,preIndex,
				    " 11 -12 13 -14 15 -16 (-1:2:-3:4:-5:6) ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,alMat,modTemp,Out));

  // VAC Outer:
  Out=ModelSupport::getSetComposite(SMap,preIndex,
		       " 21 -22 23 -24 25 -26 (-11:12:-13:14:-15:16) ");

  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  voidCell=cellIndex-1;
  
  Out=ModelSupport::getSetComposite(SMap,preIndex," 21 -22 23 -24 25 -26 ");

  addOuterSurf(Out);
  return;
}

void 
CH4PreFlat::createLinks()
  /*!
    Create link points and surfaces
   */
{
  ELog::RegMethod RegA("CH4PreFlat","createLinks");

  const double layT(alThick+vacThick);

  FixedComp::setConnect(0,Origin-Y*(depth/2.0+layT),-Y);
  FixedComp::setConnect(1,Origin+Y*(depth/2.0+layT),Y);
  FixedComp::setConnect(2,Origin-X*(width/2.0+layT),-X);
  FixedComp::setConnect(3,Origin+X*(width/2.0+layT),X);
  FixedComp::setConnect(4,Origin-Z*(height/2.0+layT),-Z);
  FixedComp::setConnect(5,Origin+Z*(height/2.0+layT),Z);


  FixedComp::setLinkSurf(0,-SMap.realSurf(preIndex+21));
  FixedComp::setLinkSurf(1,SMap.realSurf(preIndex+22));
  FixedComp::setLinkSurf(2,-SMap.realSurf(preIndex+23));
  FixedComp::setLinkSurf(3,SMap.realSurf(preIndex+24));
  FixedComp::setLinkSurf(4,-SMap.realSurf(preIndex+25));
  FixedComp::setLinkSurf(5,SMap.realSurf(preIndex+26));
  return;
}

void
CH4PreFlat::createAll(Simulation& System,
		      const attachSystem::FixedComp& FC,
		      const size_t linkPt,
		      const size_t)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Fixed unit that connects to this moderator
    \param linkPt :: Base index
  */
{
  ELog::RegMethod RegA("CH4PreFlat","createAll");
  populate(System);

  createUnitVector(FC,linkPt);
  ELog::EM<<"Origin == "<<Origin<<" X "<<X<<" Z ="<<Z<<ELog::endDebug;
  createSurfaces(FC,linkPt);
  createObjects(System);
  createLinks();
  insertObjects(System);       
  
  return;
}
  
}  // NAMESPACE moderatorSystem
