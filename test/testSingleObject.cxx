/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   test/testSingleObject.cxx
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
#include <cmath>
#include <complex> 
#include <vector>
#include <list> 
#include <map> 
#include <set>
#include <string>
#include <algorithm>
#include <functional>
#include <numeric>
#include <iterator>
#include <boost/functional.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/array.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "mathSupport.h"
#include "support.h"
#include "MapSupport.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "localRotate.h"
#include "masterRotate.h"
#include "Triple.h"
#include "NList.h"
#include "NRange.h"
#include "Tally.h"
#include "cellFluxTally.h"
#include "pointTally.h"
#include "heatTally.h"
#include "tallyFactory.h"
#include "Transform.h"
#include "Surface.h"
#include "surfIndex.h"
#include "Quadratic.h"
#include "surfaceFactory.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FItem.h"
#include "FuncDataBase.h"
#include "SurInter.h"
#include "BnId.h"
#include "Acomp.h"
#include "Algebra.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "AlterSurfBase.h"
#include "RemoveCell.h"
#include "WForm.h"
#include "weightManager.h"
#include "ObjSurfMap.h"
#include "ModeCard.h"
#include "PhysCard.h"
#include "PhysImp.h"
#include "LSwitchCard.h"
#include "KGroup.h"
#include "SrcData.h"
#include "SrcItem.h"
#include "Source.h"
#include "KCode.h"
#include "PhysicsCards.h"
#include "ReadFunctions.h"
#include "surfRegister.h"
#include "ModelSupport.h"
#include "neutron.h"
#include "Simulation.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "ContainedComp.h"
#include "LayerComp.h"
#include "ConicInfo.h"
#include "ModBase.h"
#include "CylMod.h"
#include "World.h"
#include "LineTrack.h"

#include "CylMod.h"

#include "testFunc.h"
#include "simpleObj.h"
#include "testSingleObject.h"

testSingleObject::testSingleObject() 
  /*!
    Constructor
  */
{
}

testSingleObject::~testSingleObject() 
  /*!
    Destructor
  */
{}

void
testSingleObject::initSim()
  /*!
    Set all the objects in the simulation:
  */
{
  ASim.resetAll();
  return;
}

void 
testSingleObject::cylModVariables(FuncDataBase& Control)
  /*
    Set the variables for the lower moderators
    \param Control :: DataBase to put variables
   */
{
  ELog::RegMethod RegA("testSingleObject","cylModVariable");

  Control.addVariable("H2CylModXStep",0.0);  
  Control.addVariable("H2CylModYStep",0.0);  
  Control.addVariable("H2CylModZStep",0.0);
  Control.addVariable("H2CylModXYangle",0.0);
  Control.addVariable("H2CylModZangle",0.0);
  Control.addVariable("H2CylModRadius",6.0);
  Control.addVariable("H2CylModHeight",12.0);
  Control.addVariable("H2CylModMat",25);
  Control.addVariable("H2CylModTemp",20.0);
  Control.addVariable("H2CylModNLayers",0);
  // al layer
  Control.addVariable("H2CylModHGap1",0.3);
  Control.addVariable("H2CylModRadGap1",0.3);
  Control.addVariable("H2CylModMaterial1",5);  // Al materk
  Control.addVariable("H2CylModTemp1",20.0);  
  
  // Vac gap
  Control.addVariable("H2CylModHGap2",0.5);
  Control.addVariable("H2CylModRadGap2",0.5);
  Control.addVariable("H2CylModMaterial2",0); 
  // Next Al layer
  Control.addVariable("H2CylModHGap3",0.2);
  Control.addVariable("H2CylModRadGap3",0.2);
  Control.addVariable("H2CylModMaterial3",5); 
  Control.addVariable("H2CylModTemp3",77.0);  
  // He Layer
  Control.addVariable("H2CylModHGap4",0.2);
  Control.addVariable("H2CylModRadGap4",0.2);
  Control.addVariable("H2CylModMaterial4",0); 
  // Outer Layer
  Control.addVariable("H2CylModHGap5",0.2);
  Control.addVariable("H2CylModRadGap5",0.2);
  Control.addVariable("H2CylModMaterial5",5); 
  Control.addVariable("H2CylModTemp5",300.0); 
  // Clearance
  Control.addVariable("H2CylModHGap6",0.2);
  Control.addVariable("H2CylModRadGap6",0.2);
  Control.addVariable("H2CylModMaterial6",0); 

  Control.addVariable("H2CylModNConic",1);

  Control.addVariable("H2CylModConic1Cent",Geometry::Vec3D(0,1,0));
  Control.addVariable("H2CylModConic1Axis",Geometry::Vec3D(0,1,0));
  Control.addVariable("H2CylModConic1Angle",35.0);
  Control.addVariable("H2CylModConic1WallMat",5);
  Control.addVariable("H2CylModConic1Wall",0.0);

  Control.addVariable("H2CylModConic2Cent",Geometry::Vec3D(1,0,0));
  Control.addVariable("H2CylModConic2Axis",Geometry::Vec3D(1,0,0));
  Control.addVariable("H2CylModConic2Angle",35.0);
  Control.addVariable("H2CylModConic2WallMat",5);
  Control.addVariable("H2CylModConic2Wall",0.2);

  Control.addVariable("H2CylModConic3Cent",Geometry::Vec3D(-1,0,0));
  Control.addVariable("H2CylModConic3Axis",Geometry::Vec3D(-1,0,0));
  Control.addVariable("H2CylModConic3Angle",35.0);
  Control.addVariable("H2CylModConic3WallMat",5);
  Control.addVariable("H2CylModConic3Wall",0.2);

  Control.addVariable("H2CylModConic4Cent",Geometry::Vec3D(0,-1,0));
  Control.addVariable("H2CylModConic4Axis",Geometry::Vec3D(0,-1,0));
  Control.addVariable("H2CylModConic4Angle",35.0);
  Control.addVariable("H2CylModConic4WallMat",5);
  Control.addVariable("H2CylModConic4Wall",0.2);

  Control.addVariable("H2CylPreNLayers",4);  
  Control.addVariable("H2CylPreHeight1",0.2);  
  Control.addVariable("H2CylPreDepth1",0.2);  
  Control.addVariable("H2CylPreThick1",0.2);  
  Control.addVariable("H2CylPreMaterial1",5);  
  Control.addVariable("H2CylPreHeight2",1.0);  
  Control.addVariable("H2CylPreDepth2",2.0);  
  Control.addVariable("H2CylPreThick2",2.0);  
  Control.addVariable("H2CylPreMaterial2",11);  
  Control.addVariable("H2CylPreHeight3",0.2);  
  Control.addVariable("H2CylPreDepth3",0.2);  
  Control.addVariable("H2CylPreThick3",0.2);  
  Control.addVariable("H2CylPreMaterial3",5);  
  Control.addVariable("H2CylPreHeight4",0.2);  
  Control.addVariable("H2CylPreDepth4",0.2);  
  Control.addVariable("H2CylPreThick4",0.2);  
  Control.addVariable("H2CylPreMaterial4",0); 

  return;
}
  

int 
testSingleObject::applyTest(const int extra)
  /*!
    Applies all the tests and returns 
    the error number
    \param extra :: Test number to run
    \retval -1 : SetObject 
    \retval 0 : All succeeded
  */
{
  ELog::RegMethod RegA("testSingleObject","applyTest");

  typedef int (testSingleObject::*testPtr)();
  testPtr TPtr[]=
    {
      &testSingleObject::testLineTrackCylinder,
      &testSingleObject::testLineTrackCone
    };
  const std::string TestName[]=
    {
      "LineTrack(Cyl)",
      "LineTrack(Cone)"
    };
  
  const int TSize(sizeof(TPtr)/sizeof(testPtr));
  if (!extra)
    {
      std::ios::fmtflags flagIO=std::cout.setf(std::ios::left);
      for(int i=0;i<TSize;i++)
        {
	  std::cout<<std::setw(30)<<TestName[i]<<"("<<i+1<<")"<<std::endl;
	}
      std::cout.flags(flagIO);
      return 0;
    }
  for(int i=0;i<TSize;i++)
    {
      if (extra<0 || extra==i+1)
        {
	  TestFunc::regTest(TestName[i]);
	  const int retValue= (this->*TPtr[i])();
	  if (retValue || extra>0)
	    return retValue;
	}
    }
  return 0;
}

void
testSingleObject::createObj()
  /*!
    test the createation of the surface map
    \return -1 on failure
  */
{
  ELog::RegMethod RegA("testSingleObject","testCreateObjSurfMap");

  cylModVariables(ASim.getDataBase());

  World::createOuterObjects(ASim);
  
  ts1System::CylMod A("H2CylMod");
  A.addInsertCell(74123);
  A.createAll(ASim,World::masterOrigin());

  ASim.createObjSurfMap();
  std::vector<int> rOffset;
  std::vector<int> rRange;  
  ASim.renumberSurfaces(rOffset,rRange);
  ASim.renumberCells(rOffset,rRange);
  ASim.createObjSurfMap();

  ASim.prepareWrite();
  ASim.write("simple.x");
  return;
}

int
testSingleObject::testLineTrackCone()
  /*!
    Simple line thought system
    \return 0 on success / -ve on error
   */
{
  ELog::RegMethod RegA("testSingleObject","testLineTrackCone");
  return testLineTrack();
}

int
testSingleObject::testLineTrackCylinder()
  /*!
    Simple line thought system
    \return 0 on success / -ve on error
   */
{
  ELog::RegMethod RegA("testSingleObject","testLineTrackCylinder");
  FuncDataBase& Control=ASim.getDataBase();
  Control.addVariable("H2CylModCylFlag",1);
  return testLineTrack();
}

int
testSingleObject::testLineTrack()
  /*!
    Simple line thought system
    \return 0 on success / -ve on error
   */
{
  ELog::RegMethod RegA("testSingleObject","testLineTrack");

  initSim();
  createObj();
  
  typedef boost::tuple<Geometry::Vec3D,Geometry::Vec3D,int,double> TTYPE;
  std::vector<TTYPE> Tests;
  // First test:
  // 24 : 
  Tests.push_back(TTYPE(Geometry::Vec3D(0,2,30),
			Geometry::Vec3D(0,2,-30),
			9,2*48.0+5*12.0));

  Tests.push_back(TTYPE(Geometry::Vec3D(0.1,0.1,30),
			Geometry::Vec3D(0.1,0.1,-30),
			9,2*48.0+5*12.0));
  
  std::vector<TTYPE>::const_iterator tc;
  int flag(0);
  for(tc=Tests.begin();tc!=Tests.end() && !flag;tc++)
    {
      ModelSupport::LineTrack LT(tc->get<0>(),tc->get<1>());
      LT.calculate(ASim);
      if (!checkResult(LT,tc->get<2>(),tc->get<3>()))
	{
	  ELog::EM<<"Failed on test :"<<(tc-Tests.begin())+1<<ELog::endTrace;
	  ELog::EM<<LT<<ELog::endTrace;
	  return -1;
	}
    }
  return 0;
}

int
testSingleObject::checkResult(const ModelSupport::LineTrack& LT,
			      const int CSum,const double TSum) const
  /*!
    Check the result from the test
    \param LT :: LineTrack to test
    \param CSum :: Sum of cells
    \param TSum :: Sum of Track*CellIndex
    \return true if good
   */
{
  ELog::RegMethod RegA("testSingleObject","checkResults");

  const std::vector<int>& cells=LT.getCells();
  const std::vector<double>& tLen=LT.getTrack();
  const std::vector<MonteCarlo::Object*>& oVec=LT.getObjVec();
  int cValue(0);
  double tValue(0.0);
  for(size_t i=0;i<cells.size();i++)
    {
      if (!oVec[i] || oVec[i]->getName()!=cells[i])
	return 0;
      cValue+=cells[i];
      tValue+=tLen[i]*cells[i];
    }  
  ELog::EM<<"cvalu == "<<cValue<<" "<<tValue<<ELog::endDebug;
  ELog::EM<<"X == "<<CSum<<" "<<TSum<<ELog::endDebug;
  return (cValue!=CSum || fabs(TSum-tValue)>1e-3) ? 0 : 1;
}
