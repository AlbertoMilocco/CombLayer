/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   buildInc/TS2moly.h
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
#ifndef TMRSystem_TS2moly_h
#define TMRSystem_TS2moly_h

class Simulation;


namespace TMRSystem
{

/*!
  \struct plateCut 
  \brief Parameters for a cone cut cut
  \author S. Ansell
  \date February 2012
*/
struct plateCut
{
  Geometry::Vec3D centre;       ///< Centre
  Geometry::Vec3D axis;         ///< Axis joining centres
  double thick;                 ///< Distance between plates [total]
  int mat;                      ///< Material
  int layerMat;                 ///< Skin material
  double layerThick;            ///< Skin thickness
};

/*!
  \struct coneCut 
  \brief Parameters for a cone cut cut
  \author S. Ansell
  \date February 2012
*/
struct coneCut
{
  Geometry::Vec3D centre;       ///< Centre
  Geometry::Vec3D axis;         ///< Axis joining centres
  double angleA;                ///< Angle of cone A
  double angleB;                ///< Angle of cone B
  double dist;                  ///< Distance between radius
  int mat;                      ///< Material
  int layerMat;                 ///< Skin material
  double layerThick;            ///< Skin thickness

  /// Calc of axis for cone
  void axisCalc(const Geometry::Vec3D& A,
		const Geometry::Vec3D& B,
		const Geometry::Vec3D& C)
  {
    centre=A*centre.X()+B*centre.Y()+C*centre.Z();
    axis=A*axis.X()+B*axis.Y()+C*axis.Z();
    axis.makeUnit();
  }
  /// Accessor to flag
  int cutFlagA() const { return (angleA>0.0) ? 1  : -1; }
  /// Accessor to flag
  int cutFlagB() const { return (angleB>0.0) ? 1  : -1; }
};

/*!
  \struct spherCut
  \brief Parameters for a back-to-back sphere cut
  \author S. Ansell
  \date February 2012
  \
*/

struct sphereCut
{
  Geometry::Vec3D centre;       ///< Centre
  Geometry::Vec3D axis;         ///< Axis joining centres
  double radius;                ///< Radius
  double dist;                  ///< Distance between radius
  double negCutPlane;           ///< negative plane
  double posCutPlane;           ///< Positive   
  int mat;                      ///< Material

  /// Quick assignment of plane cuts
  void defCutPlane() { negCutPlane= posCutPlane=dist+radius;  }
  /// Placing relative centre/axis on actual origination
  void axisCalc(const Geometry::Vec3D& A,const Geometry::Vec3D& B,
		const Geometry::Vec3D& C)
  {
    centre=A*centre.X()+B*centre.Y()+C*centre.Z();
    axis=A*axis.X()+B*axis.Y()+C*axis.Z();
    axis.makeUnit();
  }
};

/*!
  \class TS2moly
  \version 1.0
  \author S. Ansell
  \date February 2012
  \brief Modification to the Target with Mo.
*/

class TS2moly :  public TS2target
{
 private:
  
  const int molyIndex;          ///< Index of surface offset
  const std::string molyKey;    ///< moly key name
  int cellIndex;                ///< Cell index

  // PLATES:
  std::vector<plateCut> PCut;  

  // Sphere-Join
  std::vector<Geometry::Vec3D> SCent;  ///< Centre point
  std::vector<double> Radius;          ///< Radius of sphere

  /// Sphere Cut:
  std::vector<sphereCut> SCut;  

  /// Cone Cut:
  std::vector<coneCut> CCut;
  
  void populate(const Simulation&);
  void createUnitVector(const attachSystem::FixedComp&);
  void createSurfaces();
  void createObjects(Simulation&);

 public:

  TS2moly(const std::string&,const std::string&);
  TS2moly(const TS2moly&);
  TS2moly& operator=(const TS2moly&);
  TS2moly* clone() const;
  virtual ~TS2moly();

  virtual void createAll(Simulation&,const attachSystem::FixedComp&);
};

}

#endif
 
