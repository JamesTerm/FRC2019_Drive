#pragma once

//as a start we'll include the same controls used in the robot libraries
#define COMMON_API
#include "../../../main/cpp/Base/Base_Includes.h"
#include "../../../main/cpp/Base/Script.h"
//#include "../../../main/cpp/Base/Time_Type.h"

#include "../../../main/cpp/Common/Robot_Control_Common.h"
#include <frc/Joystick.h>
#include "../../../main/cpp/Common/SmartDashboard.h"

namespace frc
{
	#if 0
	class VictorSP : public PWMSpeedController
	{
	public:
		/**
		 * Constructor for a VictorSP.
		 *
		 * @param channel The PWM channel that the VictorSP is attached to. 0-9 are
		 *                on-board, 10-19 are on the MXP port
		 */
		VictorSP(int channel) : PWMSpeedController(0, channel, "VictorSP")  //Note: the name will need to be merged after construction
		{
		}

		VictorSP(VictorSP&&) = default;
		VictorSP& operator=(VictorSP&&) = default;
	};
	#endif

	enum ErrorCode : int32_t
	{
		OK = 0
	};

	enum FeedbackDevice
	{
		//NOTE: None was removed as it doesn't exist in firmware
		//TODO: Add None to firmware and add None back in
		QuadEncoder = 0,
		//1
		Analog = 2,
		//3
		Tachometer = 4,
		PulseWidthEncodedPosition = 8,

		SensorSum = 9,
		SensorDifference = 10,
		RemoteSensor0 = 11,
		RemoteSensor1 = 12,
		//13
		//14
		SoftwareEmulatedSensor = 15,

		CTRE_MagEncoder_Absolute = PulseWidthEncodedPosition,
		CTRE_MagEncoder_Relative = QuadEncoder,
	};

	enum RemoteFeedbackDevice
	{
		RemoteFeedbackDevice_None = -1,

		RemoteFeedbackDevice_SensorSum = 9,
		RemoteFeedbackDevice_SensorDifference = 10,
		RemoteFeedbackDevice_RemoteSensor0 = 11,
		RemoteFeedbackDevice_RemoteSensor1 = 12,
		//13
		//14
		RemoteFeedbackDevice_SoftwareEmulatedSensor = 15,
	};
	enum class ControlMode 
	{
		PercentOutput = 0,
		Position = 1,
		Velocity = 2,
		Current = 3,
		Follower = 5,
		MotionProfile = 6,
		MotionMagic = 7,
		MotionProfileArc = 10,

		Disabled = 15,
	};

	class VictorSPX : public PWMSpeedController
	{
	public:
		VictorSPX(int channel) : PWMSpeedController(0, channel, "VictorSPX")  //Note: the name will need to be merged after construction
		{
		}
		//for derived classes
		VictorSPX(int channel, const char *name) : PWMSpeedController(0, channel, name)
		{
		}

		void Set(ControlMode mode, double Value) { PWMSpeedController::Set(Value); }
		virtual double GetMotorOutputPercent() { return 0.0; }
		virtual ErrorCode ConfigSelectedFeedbackSensor(RemoteFeedbackDevice feedbackDevice, int pidIdx, int timeoutMs) { return OK; }
		virtual ErrorCode ConfigSelectedFeedbackSensor(FeedbackDevice feedbackDevice, int pidIdx, int timeoutMs) { return OK; }
	};

	class SensorCollection {
	public:
		int GetAnalogIn() { return 0; }
		ErrorCode SetAnalogPosition(int newPosition, int timeoutMs = 0) { return OK; }
		int GetAnalogInRaw() { return 0; }
		int GetAnalogInVel() { return 0; }
		int GetQuadraturePosition() { return 0; }
		ErrorCode SetQuadraturePosition(int newPosition, int timeoutMs = 0) { return OK; }
		ErrorCode SyncQuadratureWithPulseWidth(int bookend0, int bookend1, bool bCrossZeroOnInterval, int offset = 0, int timeoutMs = 0) { return OK; }
		int GetQuadratureVelocity() { return 0; }
		int GetPulseWidthPosition() { return 0; }
		ErrorCode SetPulseWidthPosition(int newPosition, int timeoutMs = 0) { return OK; }
		int GetPulseWidthVelocity() { return 0; }
		int GetPulseWidthRiseToFallUs() { return 0; }
		int GetPulseWidthRiseToRiseUs() { return 0; }
		int GetPinStateQuadA() { return 0; }
		int GetPinStateQuadB() { return 0; }
		int GetPinStateQuadIdx() { return 0; }
		int IsFwdLimitSwitchClosed() { return 0; }
		int IsRevLimitSwitchClosed() { return 0; }
	};

	class TalonSRX : public VictorSPX
	{
	private:
		SensorCollection m_SensorCollectionPacify;
		public:
			TalonSRX(int channel) : VictorSPX(channel, "TalonSRX")  //Note: the name will need to be merged after construction
			{
			}

			SensorCollection & GetSensorCollection() {		return m_SensorCollectionPacify; }
	};


	class AnalogPotentiometer : public AnalogChannel
	{
	public:
		AnalogPotentiometer(int channel) : AnalogChannel(0,channel,"AnalogPotentiometer")
		{}
		double Get() { return (double)GetAverageValue(); }
	};

	class Encoder : public Encoder2
	{
	public:
		Encoder(int AChannel,int BChannel,bool Reversed) : Encoder2(0,AChannel,BChannel,"Encoder")
		{}
	};

	//class PowerDistributionPanel : public ErrorBase, public SendableBase {
	class PowerDistributionPanel {
	public:
		PowerDistributionPanel() {}
		explicit PowerDistributionPanel(int module) {}

		PowerDistributionPanel(PowerDistributionPanel&&) = default;
		PowerDistributionPanel& operator=(PowerDistributionPanel&&) = default;
		double GetVoltage() const { return 0.0; }
		double GetTemperature() const { return 0.0; }
		double GetCurrent(int channel) const { return 0.0; }
		double GetTotalCurrent() const { return 0.0; }
		double GetTotalPower() const { return 0.0; }
		double GetTotalEnergy() const { return 0.0; }
		void ResetTotalEnergy() {}
		void ClearStickyFaults() {}
		//void InitSendable(SendableBuilder& builder) override;
		//void InitSendable(SendableBuilder& builder) {}
	private:
		//HAL_PDPHandle m_handle = HAL_kInvalidHandle;
	};

}

//TODO implement
namespace frc {
	#if 0
	//class DriverStation : public ErrorBase {
	class DriverStation {
	public:
		DriverStation() {}
		enum Alliance { kRed, kBlue, kInvalid };
		enum MatchType { kNone, kPractice, kQualification, kElimination };

		~DriverStation() {}

		DriverStation(const DriverStation&) = delete;
		DriverStation& operator=(const DriverStation&) = delete;

		static DriverStation& GetInstance();
		//static void ReportError(const wpi::Twine& error);
		//static void ReportWarning(const wpi::Twine& error);
		//static void ReportError(bool isError, int code, const wpi::Twine& error,const wpi::Twine& location, const wpi::Twine& stack);
		static constexpr int kJoystickPorts = 6;
		bool GetStickButton(int stick, int button) { return false; }
		bool GetStickButtonPressed(int stick, int button) { return false; }
		bool GetStickButtonReleased(int stick, int button) { return false; }
		double GetStickAxis(int stick, int axis) { return 0.0; }
		int GetStickPOV(int stick, int pov) { return 0; }
		int GetStickButtons(int stick) const { return 0; }
		int GetStickAxisCount(int stick) const { return 0; }
		int GetStickPOVCount(int stick) const { return 0; }
		int GetStickButtonCount(int stick) const { return 0; }
		bool GetJoystickIsXbox(int stick) const { return false; }
		int GetJoystickType(int stick) const { return 0; }
		std::string GetJoystickName(int stick) const { return "todo"; }
		int GetJoystickAxisType(int stick, int axis) const { return 0; }
		bool IsEnabled() const { return true; }
		bool IsDisabled() const { return false; }
		bool IsAutonomous() const { return false; }
		bool IsOperatorControl() const { return false; }
		bool IsTest() const { return false; }
		bool IsDSAttached() const { return false; }
		bool IsNewControlData() const { return false; }
		bool IsFMSAttached() const { return false; }
		bool IsSysActive() const { return false; }
		bool IsBrownedOut() const { return false; }
		std::string GetGameSpecificMessage() const { return "todo"; }
		std::string GetEventName() const { return "todo"; }
		MatchType GetMatchType() const { return kNone; }
		int GetMatchNumber() const { return 0; }
		int GetReplayNumber() const { return 0; }
		Alliance GetAlliance() const { return kBlue; }
		int GetLocation() const { return 0; }
		void WaitForData() {}
		bool WaitForData(double timeout) { return false; }
		double GetMatchTime() const { return 0.0; }
		double GetBatteryVoltage() const { return 0.0; }
		void InDisabled(bool entering) { m_userInDisabled = entering; }
		void InAutonomous(bool entering) { m_userInAutonomous = entering; }
		void InOperatorControl(bool entering) { m_userInTeleop = entering; }
		void InTest(bool entering) { m_userInTest = entering; }
	protected:
		//void GetData();
	private:
		//DriverStation();
		//void ReportJoystickUnpluggedError(const wpi::Twine& message);
		//void ReportJoystickUnpluggedWarning(const wpi::Twine& message);
		//void Run();
		//void SendMatchData();

		//std::unique_ptr<MatchDataSender> m_matchDataSender;

		// Joystick button rising/falling edge flags
		//wpi::mutex m_buttonEdgeMutex;
		//std::array<HAL_JoystickButtons, kJoystickPorts> m_previousButtonStates;
		//std::array<uint32_t, kJoystickPorts> m_joystickButtonsPressed;
		//std::array<uint32_t, kJoystickPorts> m_joystickButtonsReleased;

		// Internal Driver Station thread
		//std::thread m_dsThread;
		//std::atomic<bool> m_isRunning{ false };

		//wpi::mutex m_waitForDataMutex;
		//wpi::condition_variable m_waitForDataCond;
		//int m_waitForDataCounter;

		// Robot state status variables
		bool m_userInDisabled = false;
		bool m_userInAutonomous = false;
		bool m_userInTeleop = false;
		bool m_userInTest = false;

		//double m_nextMessageTime = 0;
	};
	#endif
	int32_t HAL_SetJoystickOutputs(int32_t joystickNum, int64_t outputs, int32_t leftRumble, int32_t rightRumble);

}
