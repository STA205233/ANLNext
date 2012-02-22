/*************************************************************************
 *                                                                       *
 * Copyright (c) 2011 Hirokazu Odaka                                     *
 *                                                                       *
 * This program is free software: you can redistribute it and/or modify  *
 * it under the terms of the GNU General Public License as published by  *
 * the Free Software Foundation, either version 3 of the License, or     *
 * (at your option) any later version.                                   *
 *                                                                       *
 * This program is distributed in the hope that it will be useful,       *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 * GNU General Public License for more details.                          *
 *                                                                       *
 * You should have received a copy of the GNU General Public License     *
 * along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 *                                                                       *
 *************************************************************************/

#include "ANLNext.hh"
#include "ANLVModule.hh"
#include "EvsManager.hh"
#include "ANLAccess.hh"

#include <iomanip>
#include <algorithm>
#include <boost/format.hpp>

#if ANL_ANALYZE_INTERRUPT
#include <signal.h>
#endif

#include "ANLNext_impl.hh"

using namespace anl;

ANLNext::ANLNext()
{
  m_Evs = new EvsManager;
  m_Evs->Initialize();
}


ANLNext::~ANLNext()
{
  delete m_Evs;
}


void ANLNext::SetModules(std::vector<ANLVModule*> modules) throw(ANLException)
{
  m_Modules = modules;
  
  for (AMIter mod = m_Modules.begin(); mod != m_Modules.end(); ++mod) {
    (*mod)->set_evs_manager(m_Evs);
  }

  for (AMIter mod = m_Modules.begin(); mod != m_Modules.end(); ++mod) {
    ANLAccess anlAccess;
    
    for (AMIter r = m_Modules.begin(); r != m_Modules.end(); ++r) {
      std::string name;
      name = (*r)->module_name();
      if ( (*mod)->accessible(name) ) {
        anlAccess.RegisterModule(name, *r);
      }

      std::vector<std::string> alias = (*r)->get_alias();
      for (size_t i=0; i<alias.size(); i++) {
        name = alias[i];
        if ( (*mod)->accessible(name) ) {
          anlAccess.RegisterModule(name, *r);
        }
      }
    }

    (*mod)->set_anl_access(anlAccess);
  }
  
  m_Counter.resize(m_Modules.size());
  reset_counter();
}


ANLStatus ANLNext::Startup() throw(ANLException)
{
  std::cout << '\n'
            << "      ***********************************\n"
            << "      ****          ANL Next         ****\n"
            << "      ***********************************\n"
            << std::endl;

  return routine_startup();
}


ANLStatus ANLNext::Initialize() throw(ANLException)
{
  show_analysis();
  print_parameters();
  reset_counter();

  ANLStatus status = AS_OK;
  status = routine_init();
  if (status != AS_OK) goto final;

  status = routine_his();
  if (status != AS_OK) goto final;
  
  std::cout << "ANLNext: initialization done." << std::endl;
  
 final:
  return status;
}


ANLStatus ANLNext::Analyze(int num_event, int display_freq) throw(ANLException)
{
#if ANL_ANALYZE_INTERRUPT
  struct sigaction sa;
  struct sigaction sa_org;
  std::memset(&sa, 0, sizeof(sa));
  std::memset(&sa_org, 0, sizeof(sa_org));
  sa.sa_handler = SIG_DFL;
  sa.sa_flags |= SA_RESTART;
  if( sigaction(SIGINT, &sa, &sa_org) != 0 ) {
    std::cout << "sigaction(2) error!" << std::endl;
    return AS_QUIT;
  }
#endif

  ANLStatus status = AS_OK;
  std::cout << "ANLNext: start bgnrun routine." << std::endl;
  status = routine_bgnrun();
  if (status != AS_OK) {
    goto final;
  }

  std::cout << "ANLNext: start analysis." << std::endl;
  status = process_analysis(num_event, display_freq);
  if (status != AS_OK) {
    print_summary();
    goto final;
  }

  std::cout << "ANLNext: start endrun routine." << std::endl;
  status = routine_endrun();
  if (status != AS_OK) {
    print_summary();
    goto final;
  }

 final:

#if ANL_ANALYZE_INTERRUPT
  if( sigaction(SIGINT, &sa_org, 0) != 0 ) {
    std::cout << "sigaction(2) error!" << std::endl;
    return AS_QUIT;
  }
#endif

  std::cout << std::endl;
  print_summary();
  return status;
}


ANLStatus ANLNext::Exit() throw(ANLException)
{
  ANLStatus status = AS_OK;
  status = routine_exit();
  if (status != AS_OK) {
    return status;
  }

  std::cout << std::endl;
  m_Evs->PrintSummary();

  std::cout << "\nANLNext: exiting..." << std::endl;
  
  return status;
}


ANLStatus ANLNext::Prepare() throw(ANLException)
{
  return routine_prepare();
}


int ANLNext::getModuleNumber(const std::string& name, bool strict)
{
  int modNumber = -1;
  if (strict) {
    for (size_t i=0; i<m_Modules.size(); ++i) {
      if (m_Modules[i]->module_name()==name) {
        if (modNumber==-1) {
          modNumber = i+1;
        }
        else {
          return -2;
        }
      }
    }
  }
  else {
    for (size_t i=0; i<m_Modules.size(); ++i) {
      std::string name2(name);
      std::transform(name2.begin(), name2.end(), name2.begin(), ANLToLower());
      std::string moduleNameLower(m_Modules[i]->module_name());
      std::transform(moduleNameLower.begin(), moduleNameLower.end(),
                     moduleNameLower.begin(), ANLToLower());
      if (moduleNameLower.find(name2)==0) {
        if (modNumber==-1) {
          modNumber = i+1;
        }
        else {
          return -2;
        }
      }
    }
  }
  return modNumber;
}


void ANLNext::show_analysis()
{
  std::cout << '\n'
            << "      ***********************************\n"
            << "      ****      Analysis chain      *****\n"
            << "      ***********************************\n"
            << std::endl;
  
  if (m_Modules.size() < 1) {
    std::cout << "No analysis chain is defined. " << std::endl;
  }
  else {
    std::cout
      << " ID " << "    "
      << "            Module Name               " << "  " 
      << " Version " << "  " << " ON/OFF \n"
      << "--------------------------------------------------------------------"
      << std::endl;
    for(size_t i = 0; i < m_Modules.size(); i++) {
      std::cout
        << std::right << std::setw(4) << i+1 << "    "
        << std::left << std::setw(40) << m_Modules[i]->module_name() << "  "
        << std::left << std::setw(9) << m_Modules[i]->module_version() << "  "
        << std::left << std::setw(8) << (m_Modules[i]->is_on() ? "ON" : "OFF")
        << '\n';
    }
  }
  std::cout << std::right << std::setw(0) << std::endl;
}


void ANLNext::print_parameters()
{
  std::cout << '\n'
            << "      ***********************************\n"
            << "      ****     Module parameters     ****\n"
            << "      ***********************************\n"
            << std::endl;
  
  AMIter const mod_end = m_Modules.end();
  for (AMIter mod = m_Modules.begin(); mod != mod_end; ++mod) {
    std::cout << "--- " << (*mod)->module_name() << " ---"<< std::endl;
    (*mod)->print_parameters();
    std::cout << std::endl;
  }
}


void ANLNext::reset_counter()
{
  for (size_t i=0; i<m_Counter.size(); i++) {
    m_Counter[i].entry = 0;
    m_Counter[i].ok = 0;
    m_Counter[i].err = 0;
    m_Counter[i].skip = 0;
    m_Counter[i].quit = 0;
  }
}


ANLStatus ANLNext::process_analysis(int num_event, int display_freq)
{
  ANLStatus status = AS_OK;
  int iEvent = 0;
  AMIter mod = m_Modules.begin();
  AMIter const mod_end = m_Modules.end();
  int iModule = 0;

  if (display_freq==0) {
    display_freq = 1;
  }

  while (iEvent != num_event) {
    if (iEvent%display_freq == 0) {
      std::cout << "Event : " << std::setw(10) << iEvent << std::endl;
      std::cout.width(0);
    }

    m_Evs->ResetAll();
    mod = m_Modules.begin(); iModule = 0;
    while (mod != mod_end) {
      if ((*mod)->is_on()) {
        ++m_Counter[iModule].entry;
        
        try {
          status = (*mod)->mod_ana();
        }
        catch (const ANLException& ex) {
          ex << ANLErrModFnInfo( (*mod)->module_name() + "::mod_ana" );
          ex << ANLErrEventIDInfo(iEvent);
          throw;
        }

        if (status == AS_OK ) {
          ++m_Counter[iModule].ok;
        }
        else {
          break;
        }
      }
      ++mod; ++iModule;
    }
    
    if (status == AS_OK) {
      m_Evs->Count();
    }
    else if(status == AS_SKIP) {
      ++m_Counter[iModule].skip;
      m_Evs->Count();
    }
    else if (status == AS_SKIP_ERR) {
      ++m_Counter[iModule].skip;
      ++m_Counter[iModule].err;
    }
    else if (status == AS_QUIT) {
      ++m_Counter[iModule].quit;
      m_Evs->Count();
      break;
    }
    else if (status == AS_QUIT_ERR) {
      ++m_Counter[iModule].quit;
      ++m_Counter[iModule].err;
      break;
    }
    else {
      break;
    }
       
    ++iEvent;
  }

  if (status==AS_SKIP_ERR || status==AS_QUIT_ERR) return status;
  
  return AS_OK;
}


void ANLNext::print_summary()
{
  const size_t n = m_Modules.size();
  std::cout << "      ***********************************\n"
            << "      ****      Analysis chain      *****\n"
            << "      ***********************************\n"
            << "               PUT: " << m_Counter[0].entry << '\n'
            << "                |\n";
  for (size_t i=0; i<n; i++) {
    std::string moduleID = m_Modules[i]->module_name() +
      "  version  " + m_Modules[i]->module_version();
    std::cout << boost::format("     [%3d]  %-40s") % i % moduleID;
    if (m_Counter[i].quit>1) { std::cout << "  ---> Quit"; }
    std::cout <<  '\n';
    std::cout << "    " << std::setw(10) << m_Counter[i].entry << "  |"
              << "  OK: " << m_Counter[i].ok
              << "  SKIP: " << m_Counter[i].skip
              << "  ERR: " << m_Counter[i].err << '\n';
    std::cout.width(0);
  }
  std::cout << "               GET: " << m_Counter[n-1].ok << '\n';
  std::cout << std::endl;
}


ANLStatus ANLNext::routine_startup()
{
  return routine_modfn(&ANLVModule::mod_startup, "startup");
}


ANLStatus ANLNext::routine_prepare()
{
  return routine_modfn(&ANLVModule::mod_prepare, "prepare");
}


ANLStatus ANLNext::routine_init()
{
  return routine_modfn(&ANLVModule::mod_init, "init");
}


ANLStatus ANLNext::routine_his()
{
  return routine_modfn(&ANLVModule::mod_his, "his");
}


ANLStatus ANLNext::routine_bgnrun()
{
  return routine_modfn(&ANLVModule::mod_bgnrun, "bgnrun");
}


ANLStatus ANLNext::routine_endrun()
{
  return routine_modfn(&ANLVModule::mod_endrun, "endrun");
}


ANLStatus ANLNext::routine_exit()
{
  return routine_modfn(&ANLVModule::mod_exit, "exit");
}
