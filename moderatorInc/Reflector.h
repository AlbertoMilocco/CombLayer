/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   moderatorInc/Reflector.h
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
#ifndef moderatorSystem_Reflector_h
#define moderatorSystem_Reflector_h

class Simulation;

namespace TMRSystem
{
  class TS2target;
  class TS2moly;
}

namespace moderatorSystem
{

/*!
  \class Reflector
  \version 1.0
  \author S. Ansell
  \date April 2011
  \brief Reflector [insert object]
*/

class Reflector : public attachSystem::ContainedComp,
    public attachSystem::FixedComp
{
 private:
  
  const int refIndex;           ///< Index of surface offset
  int cellIndex;                ///< Cell index
  
  double xStep;                 ///< Offset on X to Target
  double yStep;                 ///< Offset on Y to Target [+ve forward]
  double zStep;                 ///< Offset on Z top Target

  double xyAngle;               ///< Angle 
  double xySize;                ///< Left/Right size
  double zSize;                 ///< Vertical size
  double cutSize;               ///< End cuts

  int defMat;                   ///< Default material

  TMRSystem::TS2target* TarObj;             ///< Target object

  // TO BE MOVED INTO COUPLED OBJECT:
  boost::shared_ptr<Groove> GrooveObj;        ///< Groove Moderator
  boost::shared_ptr<Hydrogen> HydObj;         ///< Hydrogen moderator
  boost::shared_ptr<VacVessel> VacObj;        ///< Vac Vessel round G/H
  boost::shared_ptr<FlightLine> FLgroove;     ///< Groove flight line 
  boost::shared_ptr<FlightLine> FLhydro;      ///< Hydrogen flight line
  boost::shared_ptr<PreMod> PMgroove;         ///< Groove Pre mod
  boost::shared_ptr<PreMod> PMhydro;          ///< Hydro Pre mod
  boost::shared_ptr<HWrapper> Horn;           ///< Hydro Horn wrapper

  boost::shared_ptr<Decoupled> DMod;         ///< Decoupled moderator
  boost::shared_ptr<VacVessel> DVacObj;       ///< Vac Vessel round 
  boost::shared_ptr<FlightLine> FLwish;       ///< Wish [broad] flight line
  boost::shared_ptr<FlightLine> FLnarrow;     ///< Narrow flight line
  boost::shared_ptr<PreMod> PMdec;            ///< Pre-moderator for decoupled
  boost::shared_ptr<RefCutOut> IRcut;         ///< Reflector cut out for chipIR
  boost::shared_ptr<Bucket> CdBucket;         ///< Cd Bucket

  // The pads
  std::vector<moderatorSystem::CoolPad> Pads;

  void processDecoupled(Simulation&,const mainSystem::inputParam&);

  void populate(const Simulation&);
  void createUnitVector();

  void createSurfaces();
  void createInternalObjects(Simulation&,const mainSystem::inputParam&);
  void createObjects(Simulation&);
  void createLinks(const Geometry::Vec3D&,const Geometry::Vec3D&);

  void setTarget(const mainSystem::inputParam&);

 public:

  Reflector(const std::string&);
  Reflector(const Reflector&);
  Reflector& operator=(const Reflector&);
  virtual ~Reflector();

  int calcModeratorPlanes(const int,std::vector<int>&,int&) const;
  Geometry::Vec3D getViewOrigin(const int) const;
    
  void insertPipeObjects(Simulation&,const mainSystem::inputParam&);

  virtual std::string getExclude() const;

  void createAll(Simulation&,const mainSystem::inputParam&);


};

}

#endif
 
