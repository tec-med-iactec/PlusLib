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

#include "PlusConfigure.h"
#include "vtkPlusDataCollector.h"
#include "vtkPlusTEQ1Command.h"
#include "InfraredTEQ1Cam/vtkInfraredTEQ1Cam.h"

#include "vtkPlusChannel.h"
#include "vtkPlusCommandProcessor.h"

//----------------------------------------------------------------------------
static const std::string GET_TEQ1_CALIBRATION_CMD = "TEQ1Calibration";

//----------------------------------------------------------------------------

vtkStandardNewMacro(vtkPlusTEQ1Command);

//----------------------------------------------------------------------------

vtkPlusTEQ1Command::vtkPlusTEQ1Command()
  :vtkPlusCommand()
{
}

//----------------------------------------------------------------------------
vtkPlusTEQ1Command::~vtkPlusTEQ1Command()
{
}

//----------------------------------------------------------------------------
void vtkPlusTEQ1Command::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//----------------------------------------------------------------------------
void vtkPlusTEQ1Command::GetCommandNames(std::list<std::string>& cmdNames)
{
  cmdNames.clear();
  cmdNames.push_back(GET_TEQ1_CALIBRATION_CMD);
}

//----------------------------------------------------------------------------
std::string vtkPlusTEQ1Command::GetDescription(const std::string& commandName)
{
  std::string desc;
  if (commandName.empty() || igsioCommon::IsEqualInsensitive(commandName,  GET_TEQ1_CALIBRATION_CMD))
  {
    desc += GET_TEQ1_CALIBRATION_CMD;
    desc += ": Calibate the Thermal Expert Q1 camera. The TEQ1 calibration updates the offset (ShutterCalibration).";
  }
  return desc;
}

//----------------------------------------------------------------------------
void vtkPlusTEQ1Command::SetNameToTEQ1Calibration()
{
  this->SetName(GET_TEQ1_CALIBRATION_CMD);
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusTEQ1Command::ReadConfiguration(vtkXMLDataElement* aConfig)
{
  if (vtkPlusCommand::ReadConfiguration(aConfig) != PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }

  XML_READ_STRING_ATTRIBUTE_OPTIONAL(TEQ1DeviceId, aConfig);

  if (this->TEQ1DeviceId.empty())
  {
    LOG_ERROR("TEQ1DeviceId is not specified. Aborting.");
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusTEQ1Command::WriteConfiguration(vtkXMLDataElement* aConfig)
{
  if (vtkPlusCommand::WriteConfiguration(aConfig) != PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }

  XML_WRITE_STRING_ATTRIBUTE_IF_NOT_EMPTY(TEQ1DeviceId, aConfig);

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusTEQ1Command::Execute()
{
  LOG_DEBUG("vtkPlusTEQ1Command::Execute: " << (!this->Name.empty() ? this->Name : "(undefined)")
            << ", device: " << (this->TEQ1DeviceId.empty() ? "(undefined)" : this->TEQ1DeviceId));

  if (this->Name.empty())
  {
    this->QueueCommandResponse(PLUS_FAIL, "Thermal Expert Q1 command failed, no command name specified");
    return PLUS_FAIL;
  }

  vtkInfraredTEQ1Cam* TEQ1Device = this->GetTEQ1Device();
  if (nullptr == TEQ1Device)
  {
    this->QueueCommandResponse(PLUS_FAIL, std::string("Thermal expert Q1 command failed: device ")
                               + (this->TEQ1DeviceId.empty() ? "(undefined)" : this->TEQ1DeviceId) + " is not found");
    return PLUS_FAIL;
  }

  if (igsioCommon::IsEqualInsensitive(this->Name, GET_TEQ1_CALIBRATION_CMD))
  {
    LOG_INFO("Calibrating the Thermal Expert Q1: " << this->GetTEQ1DeviceId());

    if (PLUS_SUCCESS != TEQ1Device->CalibrationTEQ1Camera())
    {
      this->QueueCommandResponse(PLUS_FAIL, "vtkPlusTEQ1Command::Execute: failed, failed to calibrate TEQ1 camera");
      return PLUS_FAIL;
    }

    this->QueueCommandResponse(PLUS_SUCCESS, "Thermal Expert Q1 camera was calibrated");
    return PLUS_SUCCESS;
  }
  this->QueueCommandResponse(PLUS_FAIL, "vtkPlusTEQ1Command::Execute: failed, unknown command name: " + this->Name);
  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
vtkInfraredTEQ1Cam* vtkPlusTEQ1Command::GetTEQ1Device()
{
  vtkPlusDataCollector* dataCollector = GetDataCollector();
  if (nullptr == dataCollector)
  {
    LOG_ERROR("Data collector is invalid");
    return nullptr;
  }

  if (!this->GetTEQ1DeviceId().empty())
  {
    // TEQ1 device ID is specified
    vtkPlusDevice* device = nullptr;
    if (PLUS_SUCCESS != dataCollector->GetDevice(device, TEQ1DeviceId))
    {
      LOG_ERROR("No Thermal Expert Q1 device has been found by the name " << this->GetTEQ1DeviceId());
      return nullptr;
    }
    // device found
    vtkInfraredTEQ1Cam* TEQ1Device = vtkInfraredTEQ1Cam::SafeDownCast(device);
    if (nullptr == TEQ1Device)
    {
      LOG_ERROR("The specified device " << this->GetTEQ1DeviceId() << " is not Thermal Expert Q1 Device");
      return nullptr;
    }
    return TEQ1Device;
  }
  else
  {
    // No TEQ1 device id is specified, auto-detect the first one and use that
    for (DeviceCollectionConstIterator it = dataCollector->GetDeviceConstIteratorBegin(); it != dataCollector->GetDeviceConstIteratorEnd(); ++it)
    {
      vtkInfraredTEQ1Cam* TEQ1Device = vtkInfraredTEQ1Cam::SafeDownCast(*it);
      if (nullptr != TEQ1Device)
      {
        SetTEQ1DeviceId(TEQ1Device->GetDeviceId());
        return TEQ1Device;
      }
    }
  }
  LOG_ERROR("No Thermal Expert Q1 device has been found");
  return nullptr;
}
