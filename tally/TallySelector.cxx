/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   tally/TallySelector.cxx
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
#include <iterator>
#include <boost/array.hpp>
#include <boost/shared_ptr.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Tensor.h"
#include "Vec3D.h"
#include "Triple.h"
#include "support.h"
#include "NRange.h"
#include "NList.h"
#include "Tally.h"
#include "TallyCreate.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "MainProcess.h"
#include "inputParam.h"
#include "TallySelector.h" 
#include "basicConstruct.h"
#include "pointConstruct.h"
#include "meshConstruct.h"
#include "fluxConstruct.h"
#include "heatConstruct.h"
#include "itemConstruct.h"
#include "surfaceConstruct.h"
#include "tallyConstructFactory.h"
#include "tallyConstruct.h"

int
tallySelection(Simulation& System,const mainSystem::inputParam& IParam)
  /*!
    An amalgumation of values to determine what sort of tallies to put
    in the system.
    \param System :: Simulation to add tallies
    \param IP :: InputParam
    \return flag to indicate that more work is required after renumbering
  */
{
  ELog::RegMethod RegA("TallySelector","tallySelection(basic)");

  tallySystem::tallyConstructFactory FC;
  tallySystem::tallyConstruct TallyBuilder(FC);

  return TallyBuilder.tallySelection(System,IParam);
}
 

void
tallyModification(Simulation& System,
		  const mainSystem::inputParam& IParam)
  /*!
    Applies a large number of modifications to the tally system
    \param System :: Simulation to get tallies from 
    \param IParam :: Parameters
  */
{
  ELog::RegMethod RegA("TallySelector","tallyModification");
  const size_t nP=IParam.grpCnt("TMod");
  for(size_t i=0;i<nP;i++)
    {

      std::vector<std::string> StrItem;
      // This is enforced a >1
      const size_t nV=IParam.itemCnt("TMod",i);
      const std::string key=
	IParam.getCompValue<std::string>("TMod",i,0);
      for(size_t j=1;j<nV;j++)
	StrItem.push_back
	  (IParam.getCompValue<std::string>("TMod",i,j));

      int errFlag(1);
      if(key=="help")
	{
	  ELog::EM<<"TMod Help "<<ELog::endBasic;
	  ELog::EM<<" -- particle {tallyNumber} [oldtype] [newtype] \n"
		  <<"    Change the particle on a tally to the new type\n";
	  ELog::EM<<" -- nowindow [tallyNum]\n"
		  <<"    Remove the window on an f5 tally\n";
	  ELog::EM<<" -- energy {tallyNumber} [string] \n";
	  ELog::EM<<" -- time {tallyNumber} [string] ";
	  ELog::EM<<" -- divide {tallyNumber} [xPts,yPts] : Split tally into "
	            " multiple pieces"
		  <<ELog::endBasic;
	  ELog::EM<<ELog::endErr;
	  errFlag=0;
	}
      else if(key=="particle" && (nV==3 || nV==4))
	{
	  int tNumber(0);
	  const size_t flag((nV==4) ? 1 : 0);
	  if (flag && !StrFunc::convert(StrItem[0],tNumber))
	    ELog::EM<<"Failed to convert tally number "<<ELog::endErr;	  
	  ELog::EM<<"Changing particle "<<StrItem[flag]<<" "
		  <<StrItem[flag+1]<<ELog::endDebug;
	  tallySystem::changeParticleType(System,tNumber,
					      StrItem[flag],
					      StrItem[flag+1]);
	  errFlag=0;
	}
      else if (key=="energy")
	{
	  int tNumber(0);
	  if (nV>=2)
	    {
	      const std::string TN=
		IParam.getCompValue<std::string>("TMod",i,1);
	      if (!StrFunc::convert(StrItem[0],tNumber))
		ELog::EM<<"Failed to understand TNumber :"
			<<StrItem[0]<<ELog::endErr;
	      else
		{
		  if (tallySystem::setEnergy(System,tNumber,StrItem[1]))
		    {
		      ELog::EM<<"Error setting tally energy "<<
			StrItem[1]<<ELog::endErr;
		    }
		  errFlag=0;
		}
	    }
	}
      else if (key=="time")
	{
	  int tNumber(0);
	  if (nV>=2)
	    {
	      const std::string TN=
		IParam.getCompValue<std::string>("TMod",i,1);
	      if (!StrFunc::convert(StrItem[0],tNumber))
		ELog::EM<<"Failed to understand TNumber :"
			<<StrItem[0]<<ELog::endErr;
	      else
		{
		  if (tallySystem::setTime(System,tNumber,StrItem[1]))
		    {
		      ELog::EM<<"Error setting tally time "<<
			StrItem[1]<<ELog::endErr;
		    }
		  errFlag=0;
		}
	    }
	}
      else if (key=="nowindow")
	{
	  int tNumber(0);
	  if (nV==2)
	    {
	      const std::string TN=
		IParam.getCompValue<std::string>("TMod",i,1);
	      if (!StrFunc::convert(TN,tNumber))
		ELog::EM<<"Failed to understand TNumber :"<<TN<<ELog::endErr;
	      else
		tallySystem::removeF5Window(System,tNumber);

	      errFlag=0;
	    }
	}
      else if (key=="divide")
	{
	  int tNumber(0);
	  int xPts,yPts;
	  if (nV==4)
	    {
	      if (!StrFunc::convert(StrItem[0],tNumber) || 
		  !StrFunc::convert(StrItem[1],xPts) || 
		  !StrFunc::convert(StrItem[2],yPts) )
		{
		  ELog::EM<<"Failed to understand divide input :"
			  <<StrItem[0]<<":"<<StrItem[1]
			  <<":"<<StrItem[2]<<ELog::endErr;
		}
	      else
		tallySystem::divideF5Tally(System,tNumber,xPts,yPts);
	      errFlag=0;
	    }
	  else if (nV==3)
	    {
	      if (!StrFunc::convert(StrItem[0],xPts) || 
		  !StrFunc::convert(StrItem[1],yPts) )
		{
		  ELog::EM<<"Failed to understand divide input :"
			  <<StrItem[0]<<":"<<StrItem[1]<<ELog::endErr;
		}
	      else
		tallySystem::divideF5Tally(System,0,xPts,yPts);
	      errFlag=0;
	    }
	}
      if (errFlag)
	ELog::EM<<"Failed to process TMod : "<<key<<ELog::endErr;
    }
  return;
}

void
tallyRenumberWork(Simulation& System,
		  const mainSystem::inputParam& IParam)
  /*!
    An amalgumation of values to determine what sort of tallies to put
    in the system.
    \param System :: Simulation to add tallies
    \param IP :: InputParam
  */
{
  ELog::RegMethod RegA("TallySelector","tallyRenumberWork");


  tallySystem::tallyConstructFactory FC;
  tallySystem::tallyConstruct TallyBuilder(FC);
  TallyBuilder.tallyRenumber(System,IParam);
 
  return;
}


