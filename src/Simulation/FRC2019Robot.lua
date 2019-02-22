
Pi=3.14159265358979323846
Pi2=Pi*2
Inches2Meters=0.0254
Feet2Meters=0.3048
Meters2Feet=3.2808399
Meters2Inches=39.3700787
Inches2Meters=0.0254
OunceInchToNewton=0.00706155183333
Pounds2Kilograms=0.453592
Deg2Rad=(1/180) * Pi


g_wheel_diameter_in=6   --This will determine the correct distance try to make accurate too
WheelBase_Width_In=22.3125	  --The wheel base will determine the turn rate, must be as accurate as possible!
WheelBase_Length_In=9.625
WheelTurningDiameter_In= ( (WheelBase_Width_In * WheelBase_Width_In) + (WheelBase_Length_In * WheelBase_Length_In) ) ^ 0.5
HighGearSpeed = (749.3472 / 60.0) * Pi * g_wheel_diameter_in * Inches2Meters  * 0.9  --RPMs from BHS2015 Chassis.SLDASM
LowGearSpeed  = (346.6368 / 60.0) * Pi * g_wheel_diameter_in * Inches2Meters  * 0.9
GearSpeedRPM = 600  --this had 372.63 but this seemed too slow according to the encoder readings
GearSpeed = (GearSpeedRPM / 60.0) * Pi * g_wheel_diameter_in * Inches2Meters

Drive_MaxAccel=4
skid=math.cos(math.atan2(WheelBase_Length_In,WheelBase_Width_In))
gMaxTorqueYaw = (2 * Drive_MaxAccel * Meters2Inches / WheelTurningDiameter_In) * skid

MainRobot = {
	version = 1.2,
	control_assignments =
	{
		--by default module is 1, so only really need it for 2
		victor_spx =
		{
			id_1 = { name= "right_drive_1", channel=1}, 
			id_2 = { name= "right_drive_2", channel=2}, 
			id_3 = { name="left_drive_1", channel=3},
			id_4 = { name="left_drive_2", channel=4},
			--id_5 = { name="left_drive_3", channel=5},
			--id_6 = { name= "right_drive_3", channel=6}, 
			--id_7 = { name="arm", channel=7}
			id_5 = { name="arm", channel=7},
			--If we decide we need more power we can assign these
		},
		victor_sp =
		{
			id_1 = { name="roller", channel=8}
		},
		-- relay =
		-- {
		-- 	id_1 = { name= "CameraLED", channel=1}
		-- },
		double_solenoid =
		{
			id_1 = { name="wedge",    forward_channel=2, reverse_channel=1},
			id_2 = { name="intake",    forward_channel=3, reverse_channel=4},
			id_3 = { name="hatch",    forward_channel=5, reverse_channel=6},
			id_4 = { name="hatch_grab",    forward_channel=7, reverse_channel=8},
		},
		--These channels must be unique to digital input encoder channels as well
		--Also ensure you do not use the slot for the compressor ;)
		digital_input =
		{
			id_1 = { name="elevator_min",  channel=5},
			id_2 = { name="elevator_max",  channel=6}
		},
		analog_input =
		{
			id_1 = { name="arm_pot",  channel=2},
		},
		--encoder names must be the same name list from the victor (or other speed controls)
		--These channels must be unique to digital input channels as well
		digital_input_encoder =
		{	
			id_1 = { name= "left_drive_1",  a_channel=3, b_channel=4},
			id_2 = { name="right_drive_1",  a_channel=1, b_channel=2},
		},
		compressor	=	{ relay=8, limit=14 },
		--accelerometer	=	{ gRange=1 }
	},
	--Version helps to identify a positive update to lua
	--version = 1;
	
	Mass = 25, -- Weight kg
	MaxAccelLeft = 20, MaxAccelRight = 20, 
	MaxAccelForward = Drive_MaxAccel, MaxAccelReverse = Drive_MaxAccel, 
	MaxAccelForward_High = Drive_MaxAccel * 2, MaxAccelReverse_High = Drive_MaxAccel * 2, 
	MaxTorqueYaw =  gMaxTorqueYaw,  --Note Bradley had 0.78 reduction to get the feel
	MaxTorqueYaw_High = gMaxTorqueYaw * 5,
	MaxTorqueYaw_SetPoint = gMaxTorqueYaw * 2,
	MaxTorqueYaw_SetPoint_High = gMaxTorqueYaw * 10,
	rotation_tolerance=Deg2Rad * 2,
	rotation_distance_scalar=1.0,

	MAX_SPEED = HighGearSpeed,
	ACCEL = 10,    -- Thruster Acceleration m/s2 (1g = 9.8)
	BRAKE = ACCEL,     -- Brake Deceleration m/s2 (1g = 9.8)
	-- Turn Rates (radians/sec) This is always correct do not change
	heading_rad = (2 * HighGearSpeed * Meters2Inches / WheelTurningDiameter_In) * skid,
	
	Dimensions =
	{ Length=0.9525, Width=0.6477 }, --These are 37.5 x 25.5 inches (This is not used except for UI ignore)
	
	tank_drive =
	{
		is_closed=0,
		max_speed=HighGearSpeed,
		show_pid_dump='no',
		--we should turn this off in bench mark testing
		use_aggressive_stop=1,  --we are in small area want to have responsive stop
		ds_display_row=-1,
		wheel_base_dimensions =
		{length_in=WheelBase_Length_In, width_in=WheelBase_Width_In},
		
		--This encoders/PID will only be used in autonomous if we decide to go steal balls
		wheel_diameter_in = g_wheel_diameter_in,
		left_pid=
		{p=200, i=0, d=50},
		right_pid=
		{p=200, i=0, d=50},					--These should always match, but able to be made different
		latency=0.0,
		heading_latency=0.0,
		drive_to_scale=0.50,
		left_max_offset=0.0 , right_max_offset=0.0,   --Ensure both tread top speeds are aligned
		--This is obtainer from encoder RPM's of 1069.2 and Wheel RPM's 427.68 (both high and low have same ratio)
		encoder_to_wheel_ratio=1.0,			--example if encoder spins at 1069.2 multiply by this to get 427.68 (for the wheel rpm)
		voltage_multiply=-1.0,				--May be reversed using -1.0
		reverse_steering='no',
		left_encoder_reversed=1,
		right_encoder_reversed=0,
		motor_specs =
		{
			wheel_mass=1.5,
			cof_efficiency=1.0,
			gear_reduction=5310.0/749.3472,
			torque_on_wheel_radius=Inches2Meters * 1,
			drive_wheel_radius=Inches2Meters * 2,
			number_of_motors=2,
			
			free_speed_rpm=5310.0,
			stall_torque=2.43,
			stall_current_amp=133,
			free_current_amp=2.7
		}

	},
	robot_settings =
	{
		using_ac_drive =    0,  --True enables active configuration time slices for drive
		using_ac_operator = 0, --True enables active configuration time slices for operator
		using_ac_elevator = 0,

		--These heights represent elevator height which correspond to height observed- Change as needed
		hatch_deploy_time = 0.4,
		cargo_deploy_time = 0.76,
		hatch_grab_deploy_time = 0.5,

		cargo1_height=5,
		cargo2_height=33,
		cargo3_height=61.5,   --max height (and falls short but is ok)
		arm =
		{
			is_closed=0,
			show_pid_dump='n',
			ds_display_row=-1,
			use_pid_up_only='y',  --for now make the same, but this may change
			pid_up=
			{p=100, i=0, d=0},
			pid_down=
			{p=100, i=0, d=0},
			tolerance=0.5,  --in inches
			tolerance_count=1,
			voltage_multiply=1.0,			--May be reversed
			encoder_to_wheel_ratio=1.0,
			--If using an encoder check its pulses per revolution spec
			--encoder_pulses_per_revolution=560/4,
			Arm_SetPotentiometerSafety=true,	
			-- Using speed of 14400, about 0.15 nm of torque (about 21 oz-in)
			-- 8:50 1st stage = 0.16
			-- 18:24 2nd stage = 0.75
			-- 32:52 3rd stage = 0.62
			-- 4608/62400 / 192/192= 24 / 325 final gear ratio
			-- 14400 / 60 = 240 * gr = 17.72 rps
			-- to linear is pi * d (1.26) = 3.95 inch per revolution
			-- =69.994 about 70 ips
			-- We should achieve full scale less than a second
			max_speed=70,	
			accel=100.0,						--todo find out why this has to be so much greater
			brake=10.0,
			max_accel_forward=140,			--just go with what feels right (up may need more)
			max_accel_reverse=140,
			predict_up=.200,
			predict_down=.200,
			using_range=0,					--Warning Only use range if we have a potentiometer!

			--These are in native units of the pot
			--For simulation, its estimated to take 15.56 turns to go full range, and the pot has 16.66 available so we utilize about 93% of the pot
			--So .934 * the pot range of 4054 = 3787.15, so this should be the distance between min and mix
			pot_min_limit=145,
			pot_max_limit=3787.15+145,
			pot_range_flipped='n',

			--These min/max in inch units
			max_range= 61.5,  --confirmed range
			--Note the sketch used -43.33, but tests on actual assembly show -46.12
			min_range= 0,
			pot_offset=0,  --this will vary with the limit switch and elevator slop
			starting_position=6,
			use_aggressive_stop = 'yes',
			forward_deadzone=0.37,
			reverse_deadzone=0.37,
			--TODO 775 pro, but not needed for current simulated pot
			motor_specs =
			{
				wheel_mass=Pounds2Kilograms * 16.27,
				cof_efficiency=0.2,
				gear_reduction=1.0,
				torque_on_wheel_radius=Inches2Meters * 1.0,
				drive_wheel_radius=Inches2Meters * 2.0,
				number_of_motors=2,
				
				free_speed_rpm=84.0,
				stall_torque=10.6,
				stall_current_amp=18.6,
				free_current_amp=1.8
			}
		},
		claw =
		{
			--Note: there are no encoders here so is_closed is ignored
			tolerance=0.01,					--we need good precision
			voltage_multiply=1.0,			--May be reversed
			
			max_speed=28,
			accel=112,						--These are needed and should be high enough to grip without slip
			brake=112,
			max_accel_forward=112,
			max_accel_reverse=112
		}
	},
	controls =
	{
		--slotlist = {slot_1="operator"},
		slotlist = {slot_1="driver",slot_2="operator"},
		Joystick_1 =
		{
			control = "developer",
			Joystick_SetLeftVelocity = {type="joystick_analog", key=1, is_flipped=true, multiplier=1.0, filter=0.1, curve_intensity=3.0},
			Joystick_SetRightVelocity = {type="joystick_analog", key=2, is_flipped=true, multiplier=1.0, filter=0.1, curve_intensity=3.0},
			--Analog_Turn = {type="joystick_analog", key=0, is_flipped=false, multiplier=1.0, filter=0.3, curve_intensity=1.0},
			--Joystick_SetCurrentSpeed_2 = {type="joystick_analog", key=1, is_flipped=true, multiplier=1.0, filter=0.1, curve_intensity=0.0},
			--Joystick_FieldCentric_XAxis = {type="joystick_analog", key=0, is_flipped=false, multiplier=1.0, filter=0.3, curve_intensity=1.0},
			--Joystick_FieldCentric_YAxis = {type="joystick_analog", key=1, is_flipped=true, multiplier=1.0, filter=0.1, curve_intensity=0.0},
			--FieldCentric_Enable = {type="joystick_button", key=4, on_off=false},

			--scaled down to 0.5 to allow fine tuning and a good top acceleration speed (may change with the lua script tweaks)
			POV_Turn =  {type="joystick_analog", key=8, is_flipped=false, multiplier=1.0, filter=0.0, curve_intensity=0.0},
			--Turn_180 = {type="joystick_button", key=7, on_off=false},

			Arm_SetPosRest = {type="joystick_button", key=5, keyboard='l', on_off=false},
			Arm_SetPosCargo1 = {type="joystick_button", key=6, keyboard='1', on_off=false},
			Arm_SetPosCargo2 = {type="joystick_button", key=7,keyboard='2', on_off=false},
			Arm_SetPosCargo3 = {type="joystick_button", key=8,keyboard='3', on_off=false},

			Arm_SetCurrentVelocity = {type="joystick_analog", key=5, is_flipped=false, multiplier=1.0, filter=0.1, curve_intensity=0.0},
			Arm_IntakeDeploy={type="joystick_button", key=3, keyboard='i', on_off=true},
			Arm_Advance={type="keyboard", key='k', on_off=true},
			Arm_Retract={type="keyboard", key='j', on_off=true},
			
			Claw_SetCurrentVelocity = {type="joystick_analog", key=0, is_flipped=false, multiplier=1.0, filter=0.1, curve_intensity=0.0},
			Arm_HatchDeploy =	 {type="joystick_button", key=2, keyboard='h', on_off=true},
			Arm_HatchGrabDeploy={type="joystick_button", key=4, keyboard='o', on_off=true},
			Claw_Grip =		 {type="keyboard", key='r', on_off=true},
			Claw_Squirt =	 {type="keyboard", key='t', on_off=true},
			Robot_CloseDoor= {type="joystick_button", key=1, keyboard='u', on_off=true}
		},
		
		Joystick_2 =
		{
			control = "driver",
			Joystick_SetLeftVelocity = {type="joystick_analog", key=1, is_flipped=true, multiplier=1.0, filter=0.1, curve_intensity=0.0},
			Joystick_SetRightVelocity = {type="joystick_analog", key=5, is_flipped=true, multiplier=1.0, filter=0.1, curve_intensity=0.0},
			--Analog_Turn = {type="joystick_analog", key=0, is_flipped=false, multiplier=1.0, filter=0.3, curve_intensity=1.0},
			--Joystick_SetCurrentSpeed_2 = {type="joystick_analog", key=1, is_flipped=true, multiplier=1.0, filter=0.1, curve_intensity=0.0},
			-- Joystick_FieldCentric_XAxis = {type="joystick_analog", key=0, is_flipped=false, multiplier=1.0, filter=0.3, curve_intensity=1.0},
			-- Joystick_FieldCentric_YAxis = {type="joystick_analog", key=1, is_flipped=true, multiplier=1.0, filter=0.1, curve_intensity=0.0},
			-- Analog_Turn = {type="joystick_analog", key=5, is_flipped=false, multiplier=1.0, filter=0.3, curve_intensity=1.0},
			-- FieldCentric_Enable = {type="joystick_button", key=4, on_off=false},

			--scaled down to 0.5 to allow fine tuning and a good top acceleration speed (may change with the lua script tweaks)
			-- POV_Turn =  {type="joystick_analog", key=8, is_flipped=false, multiplier=1.0, filter=0.0, curve_intensity=0.0},
			-- Turn_180 = {type="joystick_button", key=7, on_off=false},
		},

		Joystick_3 =
		{
			control = "operator",
			
			Arm_SetPosRest = {type="joystick_button", key=5, keyboard='l', on_off=false},
			Arm_SetPosCargo1 = {type="joystick_button", key=6, keyboard='1', on_off=false},
			Arm_SetPosCargo2 = {type="joystick_button", key=7,keyboard='2', on_off=false},
			Arm_SetPosCargo3 = {type="joystick_button", key=8,keyboard='3', on_off=false},
			Arm_SetCurrentVelocity = {type="joystick_analog", key=2, is_flipped=true, multiplier=1.0, filter=0.1, curve_intensity=0.0},
			Arm_IntakeDeploy={type="joystick_button", key=3, keyboard='i', on_off=true},
			Arm_Advance={type="keyboard", key='k', on_off=true},
			Arm_Retract={type="keyboard", key='j', on_off=true},
			
			Claw_SetCurrentVelocity = {type="joystick_analog", key=1, is_flipped=true, multiplier=1.0, filter=0.1, curve_intensity=0.0},
			Arm_HatchDeploy =	 {type="joystick_button", key=2, keyboard='h', on_off=true},
			Arm_HatchGrabDeploy={type="joystick_button", key=4, keyboard='o', on_off=true},
			Claw_Grip =		 {type="keyboard", key='r', on_off=true},
			Claw_Squirt =	 {type="keyboard", key='t', on_off=true},
			Robot_CloseDoor= {type="joystick_button", key=1, keyboard='u', on_off=true}
		},
	}
}

Robot2019 = MainRobot
