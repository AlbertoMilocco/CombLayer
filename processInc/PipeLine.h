/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   processInc/PipeLine.h
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
#ifndef ModelSupport_PipeLine_h
#define ModelSupport_PipeLine_h

namespace ModelSupport
{
 
class surfRegister;


  
/*!
  \class PipeLine
  \version 1.0
  \author S. Ansell
  \date July 2011
  \brief A single unit of a pipe
*/

class PipeLine 
{
 private:

  ModelSupport::surfRegister SMap; ///< Surface register
  const int pipeIndex;             ///< Index of surface offset
  const std::string keyName;       ///< KeyName
  int cellIndex;                   ///< Cell index
  
  int nCylinder;                   ///< Number of pre-cylinders
  std::vector<cylValues> CV;       ///< Cylinder Values [ one for each radius]

  std::vector<Geometry::Vec3D> Pts;  ///< Points in pipe
  std::vector<int> activeFlags;      ///< Activity flags : one for each PUnit
 
  std::vector<pipeUnit*> PUnits;     ///< pipeUnits (1 less than Pts)

  void clearPUnits();

  int createUnits(Simulation&);

 public:

  PipeLine(const std::string&);
  PipeLine(const PipeLine&);
  PipeLine& operator=(const PipeLine&);
  ~PipeLine();

  /// Debug accessor
  const std::vector<Geometry::Vec3D>& getPt() const
    { return Pts; }
      
  void setPoints(const std::vector<Geometry::Vec3D>&);  
  void addPoint(const Geometry::Vec3D&);
  void addRadius(const double,const int,const double);
  void setActive(const size_t,const int);

  void createAll(Simulation&);
    
};

}

#endif
