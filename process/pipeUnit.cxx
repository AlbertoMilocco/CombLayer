/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   process/pipeUnit.cxx
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
#include <map> 
#include <list> 
#include <set>
#include <string>
#include <algorithm>
#include <functional>
#include <numeric>
#include <iterator>
#include <boost/shared_ptr.hpp>
 
#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Triple.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "stringCombine.h"
#include "Surface.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "Rules.h"
#include "HeadRule.h"
#include "Object.h"
#include "BnId.h"
#include "Acomp.h"
#include "Algebra.h"
#include "Line.h"
#include "Qhull.h"
#include "NRange.h"
#include "NList.h"
#include "KGroup.h"
#include "Source.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "generateSurf.h"
#include "AttachSupport.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "ContainedComp.h"
#include "LineTrack.h"
#include "pipeUnit.h"

#include "localRotate.h"
#include "masterRotate.h"
#include "Debug.h"

namespace ModelSupport
{

pipeUnit::pipeUnit(const std::string& Key,const size_t Index) : 
  attachSystem::FixedComp(StrFunc::makeString(Key,Index),3),
  attachSystem::ContainedComp(),
  surfIndex(ModelSupport::objectRegister::Instance().cell(keyName)),
  cellIndex(surfIndex+1),prev(0),next(0)
 /*!
   Constructor
  */
{}

pipeUnit::pipeUnit(const pipeUnit& A) : 
  attachSystem::FixedComp(A),attachSystem::ContainedComp(A),
  surfIndex(A.surfIndex),cellIndex(A.cellIndex),prev(A.prev),
  next(A.next),APt(A.APt),BPt(A.BPt),Axis(A.Axis),
  ANorm(A.ANorm),BNorm(A.BNorm),activeFlag(A.activeFlag),
  cylVar(A.cylVar)
  /*!
    Copy constructor
    \param A :: pipeUnit to copy
  */
{}

pipeUnit&
pipeUnit::operator=(const pipeUnit& A)
  /*!
    Assignment operator
    \param A :: pipeUnit to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedComp::operator=(A);
      attachSystem::ContainedComp::operator=(A);
      cellIndex=A.cellIndex;
      APt=A.APt;
      BPt=A.BPt;
      Axis=A.Axis;
      ANorm=A.ANorm;
      BNorm=A.BNorm;
      activeFlag=A.activeFlag;
      cylVar=A.cylVar;
    }
  return *this;
}

pipeUnit::~pipeUnit()
  /*!
    Destructor
  */
{}


void
pipeUnit::setPoints(const Geometry::Vec3D& A,
		    const Geometry::Vec3D& B)
  /*!
    Allocate the points
    \param A :: First Point
    \param B :: Second point
  */
{
  ELog::RegMethod RegA("pipeUnit","setPoints");
  
  Axis=(B-A).unit();
  APt=A;
  BPt=B;
  return;
}

void
pipeUnit::calcNorm(const int PN,
		   const Geometry::Vec3D& AAxis,
		   const Geometry::Vec3D& BAxis)
   /*!
     Calculate the surface normal value from Pt on another Axis
     \param PN :: Point direction [0==Base/1==top]
     \param AAxis :: First joining axis
     \param BAxis :: second Joining Axis
   */
{
  ELog::RegMethod RegA("pipeUnit","calcNorm");

  Geometry::Vec3D Norm=(BAxis+AAxis)/2.0;
  if (PN)
    BNorm=Norm;
  else    
    ANorm=Norm;
  return;
}
  
void 
pipeUnit::checkForward()
  /*!
    Sets the next item if necessary
   */
{
  if (next)
    next->connectFrom(this);
  return;
}

void
pipeUnit::populate(const size_t AF,const std::vector<cylValues>& CV)
  /*!
    Popuate everything assuming prev / next have been set
    along with APt and BPt 
    \param AF :: Active flag
    \param CV :: Cylindrical value
  */
{
  ELog::RegMethod RegA("pipeUnit","populate");

  activeFlag=AF;
  cylVar=CV;
  
  checkForward();
  const Geometry::Vec3D& pAxis((prev) ? prev->getAxis() : Axis);
  const Geometry::Vec3D& nAxis((next) ? -next->getAxis() : -Axis);
  calcNorm(0,Axis,pAxis);
  calcNorm(1,-Axis,nAxis);

  return;
}

const Geometry::Vec3D&
pipeUnit::getPt(const int side) const
  /*!
    Return a specific connection point
    \param side :: Side of object
    \return centre connection point
  */
{
  return (!side) ? APt : BPt;
}

void
pipeUnit::connectFrom(pipeUnit* Ptr) 
  /*!
    Set the previous connection
    \param Ptr :: Pointer to use
  */
{
  prev=Ptr;
  return;
}

void
pipeUnit::connectTo(pipeUnit* Ptr) 
  /*!
    Set the connection to the next object
    \param Ptr :: Pointer to use
  */
{
  next=Ptr;
  return;
}

double
pipeUnit::getOuterRadius() const
  /*!
    Determine the outer active cylinder
    \return Radius [outer]
   */
{
  ELog::RegMethod RegA("pipeUnit","getOuterRadius");

  if (cylVar.empty()) return 0.0;
  const size_t index=getOuterIndex();
  return cylVar[index].CRadius;
}

size_t
pipeUnit::getOuterIndex() const
  /*!
    Determine the outer index
    \return Index of outer unit 
  */
{
  ELog::RegMethod RegA("pipeUnit","getOuterIndex");

  if (cylVar.empty()) 
    throw ColErr::EmptyValue<int>("cylVar vector empty");

  size_t index=cylVar.size()-1;
  if (!activeFlag) return index;
  size_t bitIndex=1 << (index+1);
  do
    {
      bitIndex>>=1;
    } while( !(bitIndex & activeFlag) && --index>0);

  return index;
}

void
pipeUnit::createSurfaces()
  /*!
    Create Surface components
  */
{
  ELog::RegMethod RegA("pipeUnit","createSurface");

  ModelSupport::buildPlane(SMap,surfIndex+5,APt,ANorm);  
  ModelSupport::buildPlane(SMap,surfIndex+6,BPt,BNorm);
  // Create cylinders
  
  size_t bitIndex(1);    // Index for flag bit [Which to build]
  for(size_t i=0;i<cylVar.size();i++)
    {
      if (!activeFlag || (activeFlag & bitIndex))
	ModelSupport::buildCylinder(SMap,surfIndex+7+static_cast<int>(i)*10,
				    APt,Axis,cylVar[i].CRadius);
      bitIndex<<=1;
    }
   
  return;
}

void
pipeUnit::createOuterObject()
  /*!
    Construct the object and the outside bracket
    for the system
  */
{
  ELog::RegMethod RegA("pipeUnit","createOuterObject");

  const size_t outerIndex=getOuterIndex();

  const int SI(surfIndex+static_cast<int>(outerIndex)*10);
  const std::string Out=
    ModelSupport::getComposite(SMap,SI,surfIndex," -7 5M 6M ");
  addOuterSurf(Out);
  return;
}

void
pipeUnit::createObjects(Simulation& System)
  /*!
    Construct the object and the outside bracket
    for the system
  */
{
  ELog::RegMethod RegA("pipeUnit","createObjects");
  
  std::string Out;
  int SI(surfIndex);
  int SIprev(0);

  size_t bitIndex(1);
  for(size_t i=0;i<cylVar.size();i++)
    {
      if (!activeFlag || (activeFlag & bitIndex))
	{
	  Out=ModelSupport::getComposite(SMap,SI,surfIndex," -7 5M 6M ");
	  if (SIprev)
	    Out+=ModelSupport::getComposite(SMap,SIprev," 7 ");
	  System.addCell(MonteCarlo::Qhull(cellIndex++,cylVar[i].MatN,
					   cylVar[i].Temp,Out));
	  SIprev=SI;
	}      
      bitIndex<<=1;
      SI+=10;
    }            
  return;
}

void
pipeUnit::insertObjects(Simulation& System) 
  /*!
    Insert the objects into the main simulation. It is separated
    from creation since we need to determine those object that 
    need to have an exclude item added to them.
    \param System :: Simulation to add object to
    \param cellIndex :: Cell index to use
  */
{
  ELog::RegMethod RegA("pipeUnit","insertObjects");

  System.populateCells();
  System.validateObjSurfMap();

  typedef std::map<int,MonteCarlo::Object*> MTYPE;
  MTYPE OMap;         // Set on index

  Geometry::Vec3D Axis(BPt-APt);
  Axis.makeUnit();
  const Geometry::Vec3D AX(Axis.crossNormal());
  const Geometry::Vec3D AY(AX*Axis);
  const double radius=getOuterRadius();

  const int nAngle(6);
  const double angleStep(2*M_PI/nAngle);
  double angle(0.0);
  Geometry::Vec3D addVec;
  for(int i=0;i<=nAngle;angle+=angleStep,i++)
    {
      addVec=(i<nAngle) 
	? AX*cos(angle)*radius+AY*sin(angle)*radius 
	: Geometry::Vec3D(0,0,0);
      
      // Calculate central track
      LineTrack LT(APt+addVec,BPt+addVec);
      LT.calculate(System);

      const std::vector<MonteCarlo::Object*>& OVec=LT.getObjVec();
      std::vector<MonteCarlo::Object*>::const_iterator oc;
      for(oc=OVec.begin();oc!=OVec.end();oc++)
	{
	  const int ONum=(*oc)->getName();
	  if (OMap.find(ONum)==OMap.end())
	    OMap.insert(MTYPE::value_type(ONum,(*oc)));
	}
    }
  
  // Add exclude string
  MTYPE::const_iterator ac;
  for(ac=OMap.begin();ac!=OMap.end();ac++)
    {
      ac->second->addSurfString(getExclude());
      ac->second->populate();
      ac->second->createSurfaceList();
    }

  return;
}


void 
pipeUnit::createAll(Simulation& System,
		    const size_t AF,
		    const std::vector<cylValues>& CV)
  /*!
    Build a pipe unit and insert it into the model
    \param System :: Simulation to 
    \param AF :: activeFlag
    \param CV :: Values for each layer
   */
{
  ELog::RegMethod RegA("pipeUnit","createAll");

  populate(AF,CV);
  createSurfaces();
  createOuterObject();
  insertObjects(System);
  createObjects(System);
  
  return;
}

} // NAMESPACE ModelSupport

