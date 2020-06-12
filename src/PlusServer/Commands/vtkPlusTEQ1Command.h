/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Instituto de Astrofísica de Canarias (IAC)
  IACTEC Tecnología Médica.
  All rights reserved. See License.txt for details.

  This file was originally developed by Carlos Luque, IAC, IACTEC
  Tecnología Médica and was supported through Capacitación Tecnológica
  IACTEC with funds provided by the Cabildo Insular de Tenerife
  (Canary Islands), Programa Tenerife Innova 2016-2021, Marco
  Estratégico de Desarrollo Insular (MEDI), cofinanced by Fondos de
  Desarrollo de Canarias (FDCAN)
=========================================================Plus=header=end*/

#ifndef __vtkPlusTEQ1Command_h
#define __vtkPlusTEQ1Command_h

#include "vtkPlusServerExport.h"

#include "vtkPlusCommand.h"

// IGSIO includes
#include <igsioCommon.h>

class vtkInfraredTEQ1Cam;

//class vtkPlusTEQ1Tracker;
/*!
  \class vtkPlusTEQ1Command
  \brief This command calibrates the Thermal expert Q1 camera.
  \ingroup PlusLibPlusServer
 */
class vtkPlusServerExport vtkPlusTEQ1Command : public vtkPlusCommand
{
public:
  static vtkPlusTEQ1Command* New();
  vtkTypeMacro(vtkPlusTEQ1Command, vtkPlusCommand);
  virtual void PrintSelf(ostream& os, vtkIndent indent);
  virtual vtkPlusCommand* Clone() { return New(); }

  /*! Executes the command  */
  virtual PlusStatus Execute();

  /*! Read command parameters from XML */
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement* aConfig);

  /*! Write command parameters to XML */
  virtual PlusStatus WriteConfiguration(vtkXMLDataElement* aConfig);

  /*! Get all the command names that this class can execute */
  virtual void GetCommandNames(std::list<std::string>& cmdNames);

  /*! Gets the description for the specified command name. */
  virtual std::string GetDescription(const std::string& commandName);

  /*! Id of the Thermal expert Q1 device */
  vtkGetStdStringMacro(TEQ1DeviceId);
  vtkSetStdStringMacro(TEQ1DeviceId);

  /*!
    Set the command to calibrate the Thermal expert Q1 camera.
  */
  void SetNameToTEQ1Calibration();

protected:

  /*!
    Helper function to get pointer to the Thermal Expert Q1 device
    If it is NULL then a pointer to the first Thermal expert Q1 device is returned.
  */
  vtkInfraredTEQ1Cam* GetTEQ1Device();

  vtkPlusTEQ1Command();
  virtual ~vtkPlusTEQ1Command();

protected:
  std::string TEQ1DeviceId;

private:
  vtkPlusTEQ1Command(const vtkPlusTEQ1Command&);
  void operator=(const vtkPlusTEQ1Command&);
};

#endif
