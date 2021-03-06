/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   t1BuildInc/ReflectRods.h
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
#ifndef ts1System_ReflectRods_h
#define ts1System_ReflectRods_h

class Simulation;

namespace constructSystem
{
  class tubeUnit;
}

namespace ts1System
{
/*!
  \class ReflectorRods
  \version 1.0
  \author S. Ansell
  \date November 2012
  \brief Plate inserted in object 
*/

class ReflectRods : public attachSystem::ContainedComp,
    public attachSystem::FixedComp
{
 private:
  
  const int rodIndex;      ///< Index of surface offset
  int cellIndex;            ///< Cell index
  int populated;            ///< Variables populated

  double zAngle;            ///< Z angle rotation
  double xyAngle;           ///< XY angle rotation

  int defMat;               ///< Material outer 
  int innerMat;               ///< Material inner rod
  int linerMat;               ///< Material liner
  Geometry::Vec3D Centre;   ///< Centre of block
  double radius;            ///< Hole radius
  double linerThick;        ///< Hole liner radius [if any]
  double centSpc;           ///< Hole-Hole centre spacing
  
  const Geometry::Plane* topSurf;    ///< Top Plane in refObj
  const Geometry::Plane* baseSurf;   ///< Base Plane in refObj

  MonteCarlo::Qhull* RefObj;              ///< Reflector object to replace
  std::vector<Geometry::Plane*> OPSurf;   ///< Planes in the Reflector Object
  std::vector<constructSystem::tubeUnit*> HoleCentre;      ///< Tubes

  void clearHoleCentre();
  void copyHoleCentre(const std::vector<constructSystem::tubeUnit*>&);
  int checkCorners(const Geometry::Plane*,
		   const Geometry::Vec3D&) const;
  std::string plateString() const;

  void populate(const Simulation&);
  void createUnitVector(const attachSystem::FixedComp&);
  void getZSurf();

  void createSurfaces();
  void createObjects(Simulation&);
  void layerProcess(Simulation&);

  void calcCentre();
  void createCentres(const Geometry::Plane*);
  void joinHoles();

 public:

  ReflectRods(const std::string&);
  ReflectRods(const ReflectRods&);
  ReflectRods& operator=(const ReflectRods&);
  ~ReflectRods();

  void setObject(MonteCarlo::Qhull* QPtr) { RefObj=QPtr; }

  void printHoles() const;
  /// Ugly set centre
  void setCentre(const Geometry::Vec3D& C) { Centre=C; }
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const size_t);

};

}

#endif
 
