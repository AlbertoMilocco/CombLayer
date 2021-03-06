/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   t1Build/t1BulkShield.cxx
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
#include <numeric>
#include <iterator>
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
#include "Tensor.h"
#include "Vec3D.h"
#include "inputParam.h"
#include "PointOperation.h"
#include "Triple.h"
#include "NRange.h"
#include "NList.h"
#include "Quaternion.h"
#include "localRotate.h"
#include "masterRotate.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfDIter.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "surfEqual.h"
#include "surfDivide.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Line.h"
#include "LineIntersectVisit.h"
#include "Rules.h"
#include "Convex.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "KGroup.h"
#include "Source.h"
#include "shutterBlock.h"
#include "SimProcess.h"
#include "SurInter.h"
#include "Simulation.h"
#include "insertInfo.h"
#include "insertBaseInfo.h"
#include "chipDataStore.h"
#include "ModelSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "SecondTrack.h"
#include "TwinComp.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "InsertComp.h" 
#include "LinearComp.h"
#include "collInsertBase.h"
#include "collInsertBlock.h"
#include "GeneralShutter.h"
#include "BlockShutter.h"
#include "BulkInsert.h"
#include "Window.h"
#include "World.h"
#include "t1CylVessel.h"
#include "t1BulkShield.h"

namespace shutterSystem
{

const int t1BulkShield::sandalsShutter(0);  // North 1
const int t1BulkShield::prismaShutter(1);  // North 2
const int t1BulkShield::surfShutter(2);  // North 3
const int t1BulkShield::crispShutter(3);  // North 4
const int t1BulkShield::loqShutter(4);  // North 5
const int t1BulkShield::irisShutter(5);  // North 6
const int t1BulkShield::polarisIIShutter(6);  // North 7
const int t1BulkShield::toscaShutter(7);  // North 8
const int t1BulkShield::hetShutter(8);  // North 9
const int t1BulkShield::mapsShutter(9);  // South 1
const int t1BulkShield::vesuvioShutter(10);  // South 2
const int t1BulkShield::sxdShutter(11);  // South 3
const int t1BulkShield::merlinShutter(12);  // South 4
const int t1BulkShield::s5Shutter(13);  // South 5
const int t1BulkShield::mariShutter(14);  // South 6
const int t1BulkShield::gemShutter(15);  // South 7
const int t1BulkShield::hrpdShutter(16);  // South 8
const int t1BulkShield::pearlShutter(17);  // South 9

t1BulkShield::t1BulkShield(const std::string& Key)  : 
  attachSystem::FixedComp(Key,3),attachSystem::ContainedComp(),
  bulkIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(bulkIndex+1),populated(0),
  numberBeamLines(18)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Key to use
  */
{}

t1BulkShield::t1BulkShield(const t1BulkShield& A) : 
  attachSystem::FixedComp(A),attachSystem::ContainedComp(A),
  bulkIndex(A.bulkIndex),cellIndex(A.cellIndex),
  populated(A.populated),numberBeamLines(A.numberBeamLines),
  GData(A.GData),BData(A.BData),vYoffset(A.vYoffset),
  voidRadius(A.voidRadius),shutterRadius(A.shutterRadius),
  innerRadius(A.innerRadius),outerRadius(A.outerRadius),
  totalHeight(A.totalHeight),totalDepth(A.totalDepth),
  ironMat(A.ironMat),shutterCell(A.shutterCell),
  innerCell(A.innerCell),outerCell(A.outerCell)
  /*!
    Copy constructor
    \param A :: t1BulkShield to copy
  */
{}

t1BulkShield&
t1BulkShield::operator=(const t1BulkShield& A)
  /*!
    Assignment operator
    \param A :: t1BulkShield to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedComp::operator=(A);
      attachSystem::ContainedComp::operator=(A);
      cellIndex=A.cellIndex;
      populated=A.populated;
      GData=A.GData;
      BData=A.BData;
      vYoffset=A.vYoffset;
      voidRadius=A.voidRadius;
      shutterRadius=A.shutterRadius;
      innerRadius=A.innerRadius;
      outerRadius=A.outerRadius;
      totalHeight=A.totalHeight;
      totalDepth=A.totalDepth;
      ironMat=A.ironMat;
      shutterCell=A.shutterCell;
      innerCell=A.innerCell;
      outerCell=A.outerCell;
    }
  return *this;
}

t1BulkShield::~t1BulkShield() 
  /*!
    Destructor
  */
{}

void
t1BulkShield::populate(const Simulation& System)
  /*!
    Populate all the variables
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("t1BulkShield","populate");

  const FuncDataBase& Control=System.getDataBase();

  vYoffset=Control.EvalVar<double>("voidYoffset");
  
  shutterRadius=Control.EvalVar<double>(keyName+"ShutterRadius");
  innerRadius=Control.EvalVar<double>(keyName+"InnerRadius");  
  outerRadius=Control.EvalVar<double>(keyName+"OuterRadius");
  totalHeight=Control.EvalVar<double>(keyName+"Roof");
  totalDepth=Control.EvalVar<double>(keyName+"Floor");
  
  ironMat=Control.EvalVar<int>(keyName+"IronMat");

  populated = 1;
  return;
}

void
t1BulkShield::createUnitVector()
  /*!
    Create the unit vectors
  */
{
  ELog::RegMethod RegA("t1BulkShield","createUnitVector");
  attachSystem::FixedComp::createUnitVector(World::masterOrigin());
  applyShift(0.0,vYoffset,0.0);
  return;
}

void
t1BulkShield::createSurfaces(const attachSystem::FixedComp& FC)
  /*!
    Create all the surfaces
    \param FC :: Fixed object of inner surface(s)
  */
{
  ELog::RegMethod RegA("t1BulkShield","createSurface");

  //
  // Top/Base
  //
  ModelSupport::buildPlane(SMap,bulkIndex+5,Origin-Z*totalDepth,Z);
  ModelSupport::buildPlane(SMap,bulkIndex+6,Origin+Z*totalHeight,Z);

  // Layers:
  ModelSupport::buildCylinder(SMap,bulkIndex+17,
			      Origin,Z,shutterRadius);
  ModelSupport::buildCylinder(SMap,bulkIndex+27,
			      Origin,Z,innerRadius);
  ModelSupport::buildCylinder(SMap,bulkIndex+37,
			      Origin,Z,outerRadius);

  // INNER LAYER:
  SMap.addMatch(bulkIndex+7,FC.getLinkSurf(0));
  return;
}

void
t1BulkShield::createShutters(Simulation& System,		      
			     const mainSystem::inputParam& IParam)
  /*!
    Construct and build all the shutters
    \param System :: Simulation object to add data
    \param IParam :: Input parameters
  */
{
  ELog::RegMethod RegA("t1BulkShield","createShutters");
  
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  const bool insertVoid(IParam.flag("exclude") &&
			IParam.compValue("E",std::string("Insert")));

  GData.clear();
  for(int i=0;i<numberBeamLines;i++)
    {
      if (insertVoid)
	GData.push_back(boost::shared_ptr<GeneralShutter>
			(new GeneralShutter(i,"shutter")));
      else if (i==sandalsShutter)
	GData.push_back(boost::shared_ptr<GeneralShutter>
			(new BlockShutter(i,"shutter","sandalsShutter")));
      else if (i==prismaShutter)
	GData.push_back(boost::shared_ptr<GeneralShutter>
			(new BlockShutter(i,"shutter","prismaShutter")));
      else if (i==surfShutter)
	GData.push_back(boost::shared_ptr<GeneralShutter>
			(new BlockShutter(i,"shutter","surfShutter")));
      else if (i==crispShutter)
	GData.push_back(boost::shared_ptr<GeneralShutter>
			(new BlockShutter(i,"shutter","crispShutter")));
      else if (i==loqShutter)
	GData.push_back(boost::shared_ptr<GeneralShutter>
			(new BlockShutter(i,"shutter","loqShutter")));
      else if (i==irisShutter)
	GData.push_back(boost::shared_ptr<GeneralShutter>
			(new BlockShutter(i,"shutter","irisShutter")));
      else if (i==polarisIIShutter)
	GData.push_back(boost::shared_ptr<GeneralShutter>
			(new BlockShutter(i,"shutter","polarisIIShutter")));
      else if (i==toscaShutter)
	GData.push_back(boost::shared_ptr<GeneralShutter>
			(new BlockShutter(i,"shutter","toscaShutter")));
      else if (i==hetShutter)
	GData.push_back(boost::shared_ptr<GeneralShutter>
			(new BlockShutter(i,"shutter","hetShutter")));
      else if (i==mapsShutter)
	GData.push_back(boost::shared_ptr<GeneralShutter>
			(new BlockShutter(i,"shutter","mapsShutter")));
      else if (i==vesuvioShutter)
	GData.push_back(boost::shared_ptr<GeneralShutter>
			(new BlockShutter(i,"shutter","vesuvioShutter")));
      else if (i==sxdShutter)
	GData.push_back(boost::shared_ptr<GeneralShutter>
			(new BlockShutter(i,"shutter","sxdShutter")));
      else if (i==merlinShutter)
	GData.push_back(boost::shared_ptr<GeneralShutter>
			(new BlockShutter(i,"shutter","merlinShutter")));
//      else if (i==s5Shutter)
//	GData.push_back(boost::shared_ptr<GeneralShutter>
//			(new BlockShutter(i,"shutter","s5Shutter")));
      else if (i==mariShutter)
	GData.push_back(boost::shared_ptr<GeneralShutter>
			(new BlockShutter(i,"shutter","mariShutter")));
      else if (i==gemShutter)
	GData.push_back(boost::shared_ptr<GeneralShutter>
			(new BlockShutter(i,"shutter","gemShutter")));
      /*
      else if (i==hrpdShutter)
	GData.push_back(boost::shared_ptr<GeneralShutter>
			(new BlockShutter(i,"shutter","hrpdShutter")));
      else if (i==pearlShutter)
	GData.push_back(boost::shared_ptr<GeneralShutter>
	(new BlockShutter(i,"shutter","pearlShutter")));		
      */
      else
	GData.push_back(boost::shared_ptr<GeneralShutter>
			(new GeneralShutter(i,"shutter")));
      // Not registered under KeyName 
      OR.addObject(StrFunc::makeString(std::string("shutter"),i),GData.back());
    }

  MonteCarlo::Qhull* shutterObj=System.findQhull(shutterCell);
  if (!shutterObj)
    throw ColErr::InContainerError<int>(shutterCell,"shutterCell");

  for(size_t i=0;i<static_cast<size_t>(numberBeamLines);i++)
    {
      GData[i]->setExternal(SMap.realSurf(bulkIndex+7),
			    SMap.realSurf(bulkIndex+17),
			    SMap.realSurf(bulkIndex+6),
			    SMap.realSurf(bulkIndex+5));

      GData[i]->setGlobalVariables(voidRadius,shutterRadius,
				   totalDepth,totalHeight);
      GData[i]->setDivide(50000);     /// ARRRHHH....
      GData[i]->createAll(System,0.0,this);    
      
      shutterObj->addSurfString(GData[i]->getExclude());
    }

  return;
}

void
t1BulkShield::createBulkInserts(Simulation& System,
				const mainSystem::inputParam&)
  /*!
    Construct and build all the bulk insert
    \param System :: Simulation to use
    \param IParam :: Input parameters
  */
{
  ELog::RegMethod RegA("t1BulkShield","createBulkInserts");

  for(int i=0;i<numberBeamLines;i++)
    {
      BData.push_back(boost::shared_ptr<BulkInsert>
		      (new BulkInsert(i,"bulkInsert")));

      BData.back()->setLayers(innerCell,outerCell);
      BData.back()->setExternal(SMap.realSurf(bulkIndex+17),
				SMap.realSurf(bulkIndex+27),
				SMap.realSurf(bulkIndex+37) );
      BData.back()->setGlobalVariables(shutterRadius,innerRadius,outerRadius);
      BData.back()->createAll(System,*GData[static_cast<size_t>(i)]);    
    }
  return;
}

void
t1BulkShield::createObjects(Simulation& System,
			  const attachSystem::ContainedComp& CC)
  /*!
    Adds the Chip guide components
    \param System :: Simulation to create objects in
    \param CC :: Excluded object to inner layers [Void vessel]
   */
{
  ELog::RegMethod RegA("t1BulkShield","createObjects");

  std::string Out;

  Out=ModelSupport::getComposite(SMap,bulkIndex,"5 -6 -17 7")+CC.getExclude();
  System.addCell(MonteCarlo::Qhull(cellIndex++,ironMat,0.0,Out));
  shutterCell=cellIndex-1;

  Out=ModelSupport::getComposite(SMap,bulkIndex,"5 -6 -27 17");
  System.addCell(MonteCarlo::Qhull(cellIndex++,ironMat,0.0,Out));
  innerCell=cellIndex-1;

  Out=ModelSupport::getComposite(SMap,bulkIndex,"5 -6 -37 27");
  System.addCell(MonteCarlo::Qhull(cellIndex++,ironMat,0.0,Out));
  outerCell=cellIndex-1;

  Out=ModelSupport::getComposite(SMap,bulkIndex,"5 -6 -37");
  addOuterSurf(Out);

  return;
}

void
t1BulkShield::processVoid(Simulation& System)
  /*!
    Adds this as the outside layer of the void
    \param System :: Simulation to obtain/add void
  */
{
  ELog::RegMethod RegA("t1BulkShield","processVoid");
  // Add void
  MonteCarlo::Qhull* Obj=System.findQhull(74123);
  if (Obj)
    Obj->procString("-1 "+getExclude());
  else
    System.addCell(MonteCarlo::Qhull(74123,0,0.0,"-1 "+getExclude()));
  return;
}

const shutterSystem::GeneralShutter* 
t1BulkShield::getShutter(const int Index) const
  /*!
    Get a shutter pointer
    \param Index :: Index of shutter to get
    \return shutter pointer
   */
{
  ELog::RegMethod RegA("t1BulkShield","getShutter");
  if (Index<0 || Index>=static_cast<int>(GData.size()))
    throw ColErr::IndexError<int>(Index,static_cast<int>(GData.size()),
				  RegA.getBase());

  return GData[static_cast<size_t>(Index)].get();
}

const shutterSystem::BulkInsert* 
t1BulkShield::getInsert(const int Index) const
  /*!
    Get an insert pointer
    \param Index :: Index of insert to get
    \return BulkInsert point 
   */
{
  ELog::RegMethod RegA("t1BulkShield","getInsert");
  if (Index<0 || Index>=static_cast<int>(BData.size()))
    throw ColErr::IndexError<int>(Index,static_cast<int>(BData.size()),
				  RegA.getBase());
  return BData[static_cast<size_t>(Index)].get();
}

void
t1BulkShield::createLinks()
  /*!
    Create links
   */
{
  ELog::RegMethod RegA("t1BulkShield","createLinks");

  FixedComp::setConnect(0,Origin,-X);
  FixedComp::setConnect(1,Origin-Z*totalDepth,-Z);
  FixedComp::setConnect(2,Origin+Z*totalHeight,Z);

  FixedComp::setLinkSurf(0,SMap.realSurf(bulkIndex+37));
  FixedComp::setLinkSurf(1,-SMap.realSurf(bulkIndex+5));  // base
  FixedComp::addLinkSurf(2,SMap.realSurf(bulkIndex+6));

  return;
}

void
t1BulkShield::createAll(Simulation& System,
			const mainSystem::inputParam& IParam,
			const t1CylVessel& CVoid)
  /*!
    Create the shutter
    \param System :: Simulation to process
    \param IParam :: Input Parameters
    \param CVoid :: Void Vessel containment
   */
{
  ELog::RegMethod RegA("t1BulkShield","createAll");

  populate(System);
  voidRadius=CVoid.getOuterRadius();
  createUnitVector();
  createSurfaces(CVoid);
  createObjects(System,CVoid);
  processVoid(System);
  createShutters(System,IParam);
  createBulkInserts(System,IParam);
  createLinks();

  return;
}


  
}  // NAMESPACE shutterSystem
