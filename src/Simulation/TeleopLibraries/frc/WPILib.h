#pragma once

//as a start we'll include the same controls used in the robot libraries
#define COMMON_API
#include "../../../main/cpp/Base/Base_Includes.h"
#include "../Base/Script.h"

#include "../../../main/cpp/Common/Robot_Control_Common.h"

namespace frc
{
	class VictorSP : public Victor
	{
	public:
		/**
		 * Constructor for a VictorSP.
		 *
		 * @param channel The PWM channel that the VictorSP is attached to. 0-9 are
		 *                on-board, 10-19 are on the MXP port
		 */
		VictorSP(int channel) : Victor(0, channel, "VictorSP")  //Note: the name will need to be merged after construction
		{
		}

		VictorSP(VictorSP&&) = default;
		VictorSP& operator=(VictorSP&&) = default;
	};

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

	class VictorSPX : public Victor
	{
	public:
		VictorSPX(int channel) : Victor(0, channel, "VictorSPX")  //Note: the name will need to be merged after construction
		{
		}
		//for derived classes
		VictorSPX(int channel, const char *name) : Victor(0, channel, name)
		{
		}

		void Set(ControlMode mode, double Value) { Victor::Set(Value); }
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
}
