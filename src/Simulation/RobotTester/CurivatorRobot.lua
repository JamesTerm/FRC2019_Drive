
Pi=3.14159265358979323846
Pi2=Pi*2
Inches2Meters=0.0254
Feet2Meters=0.3048
Meters2Feet=3.2808399
Meters2Inches=39.3700787
OunceInchToNewton=0.00706155183333
Pounds2Kilograms=0.453592
Deg2Rad=(1/180) * Pi

wheel_diameter_Curivator_in=7.95
wheel_diameter_Rabbit_in=6
g_wheel_diameter_in=wheel_diameter_Curivator_in   --This will determine the correct distance try to make accurate too
WheelBase_Width_Rabbit_In=24.52198975	  --The wheel base will determine the turn rate, must be as accurate as possible!
WheelBase_Length_Rabbit_In=28.7422  
WheelBase_Width_Curivator_In=42.26
WheelBase_Length_Curivator_In=38.46
WheelBase_Length_In=WheelBase_Length_Curivator_In
WheelBase_Width_In=WheelBase_Width_Curivator_In

WheelTurningDiameter_In= ( (WheelBase_Width_In * WheelBase_Width_In) + (WheelBase_Length_In * WheelBase_Length_In) ) ^ 0.5
DriveGearSpeed_Curivator = (255.15 / 60.0) * Pi * g_wheel_diameter_in * Inches2Meters  * 0.9
LowGearSpeed_Rabbit  = (346.6368 / 60.0) * Pi * g_wheel_diameter_in * Inches2Meters  * 0.9 
DriveGearSpeed = DriveGearSpeed_Curivator
Drive_MaxAccel=5
--Swerve wheels means no skid
skid_rabbit=math.cos(math.atan2(WheelBase_Length_In,WheelBase_Width_In))
skid_curivator=1
skid=skid_curivator
gMaxTorqueYaw = (2 * Drive_MaxAccel * Meters2Inches / WheelTurningDiameter_In) * skid
EncoderLoop=1

-- Here are some auton tests
AutonTest_DoNothing=0
AutonTest_JustMoveStraight=1
AutonTest_JustRotate=2
AutonTest_MoveRotateSequence=3
AutonTest_BoxWayPoints=4
AutonTest_TestArm=5
AutonTest_GrabSequence=6

--This is how we set up motors on JVN to mix different motors into one average for all
--This is also a lesser stress in that the gear ratio is 1
averaged_motors =
{
	wheel_mass=1.5,
	cof_efficiency=0.9,
	gear_reduction=1.0,
	torque_on_wheel_radius=Inches2Meters * 1.8,
	drive_wheel_radius=Inches2Meters * 4,
	number_of_motors=1,
	payload_mass=200 * Pounds2Kilograms,
	speed_loss_constant=0.81,
	drive_train_effciency=0.9,
	
	free_speed_rpm=263.88,
	stall_torque=34,
	stall_current_amp=84,
	free_current_amp=0.4
}

MainRobot = {
	version = 1.0;
	control_assignments =
	{
		--by default module is 1, so only really need it for 2
		victor =
		{
			id_1 = { name= "wheel_fl", channel=1, module=1}, 
			id_2 = { name= "wheel_fr", channel=2}, 
			id_3 = { name="swivel_fl", channel=3},
			id_4 = { name="swivel_fr", channel=4},
			id_5= { name="turret", channel=5},
			id_6= { name="arm", channel=6}
			--If we decide we need more power we can assign these
			--id_3 = { name= "right_drive_3", channel=3}, 
			--id_6 = { name="left_drive_3", channel=6},
		},
		relay =
		{
			id_1 = { name= "CameraLED", channel=1}
		},
		digital_input =
		{
			--These channels must be unique to digital input encoder channels as well
			--Also ensure you do not use the slot for the compressor ;)
			id_1 = { name="dart_upper_limit",  channel=5},
			id_2 = { name="dart_lower_limit",  channel=6}
		},
		analog_input =
		{
			id_1 = { name="turret_pot",  channel=3},
			id_2 = { name="arm_pot",  channel=2}
		},
		digital_input_encoder =
		{	
			--encoder names must be the same name list from the victor (or other speed controls)
			--These channels must be unique to digital input channels as well
			id_1 = { name= "left_drive_1",  a_channel=3, b_channel=4},
			id_2 = { name="right_drive_1",  a_channel=1, b_channel=2},
		},
		compressor	=	{ relay=8, limit=14 }
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

	MAX_SPEED = DriveGearSpeed,
	ACCEL = 10,    -- Thruster Acceleration m/s2 (1g = 9.8)
	BRAKE = ACCEL,
	-- Turn Rates (radians/sec) This is always correct do not change
	heading_rad = (2 * DriveGearSpeed * Meters2Inches / WheelTurningDiameter_In) * skid,
	
	Dimensions =
	{ Length=0.9525, Width=0.6477 }, --These are 37.5 x 25.5 inches (This is not used except for UI ignore)

	tank_drive =
	{
		is_closed=0,
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
		drive_to_scale=0.50,				--For 4 to 10 50% gives a 5 inch tolerance
		left_max_offset=0.0 , right_max_offset=0.0,   --Ensure both tread top speeds are aligned
		--This is obtainer from encoder RPM's of 1069.2 and Wheel RPM's 427.68 (both high and low have same ratio)
		encoder_to_wheel_ratio=0.5,			--example if encoder spins at 1069.2 multiply by this to get 427.68 (for the wheel rpm)
		voltage_multiply=1.0,				--May be reversed using -1.0
		--Note: this is only used in simulation as 884 victors were phased out, but encoder simulators still use it
		curve_voltage=
		{t4=3.1199, t3=-4.4664, t2=2.2378, t1=0.1222, c=0},
		force_voltage=
		{t4=0, t3=0, t2=0, t1=0, c=1},
		reverse_steering='no',
		 left_encoder_reversed='no',
		right_encoder_reversed='no',
		inv_max_accel = 1/15.0,  --solved empirically
		forward_deadzone_left  = 0.02,
		forward_deadzone_right = 0.02,
		reverse_deadzone_left  = 0.02,
		reverse_deadzone_right = 0.02,
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
	
	swerve_drive =
	{
		is_closed=1,
		is_closed_swivel=1,
		
		--show_pid_dump_wheel={fl=0, fr=0, rl=0, rr=0},
		--show_pid_dump_swivel={fl=0, fr=0, rl=0, rr=0},
		
		--ds_display_row=-1,
		--where length is in 5 inches in, and width is 3 on each side (can only go 390 degrees a second)
		wheel_base_dimensions =	{length_in=WheelBase_Length_In, width_in=WheelBase_Width_In},	
		
		--This encoders/PID will only be used in autonomous if we decide to go steal balls
		wheel_diameter_in = g_wheel_diameter_in,
		wheel_pid={p=200, i=0, d=50},
		swivel_pid={p=100, i=0, d=50},
		latency=0.0,
		heading_latency=0.0,
		drive_to_scale=0.50,				--For 4 to 10 50% gives a 5 inch tolerance
		--strafe_to_scale=4/20,  --In autonomous we need the max to match the max forward and reverse
		--This is obtainer from encoder RPM's of 1069.2 and Wheel RPM's 427.68 (both high and low have same ratio)
		force_voltage=
		{t4=0, t3=0, t2=0, t1=0, c=1},
		reverse_steering='no',
		inv_max_accel = 1/15.0,  --solved empirically
		motor_specs = averaged_motors,
		wheel_common =
		{
			is_closed=EncoderLoop,
			show_pid_dump='n',
			ds_display_row=-1,
			pid={p=200, i=0, d=25},
			--Note: removed in Encoder Simulator v3
			--curve_voltage=
			--{t4=3.1199, t3=-4.4664, t2=2.2378, t1=0.1222, c=0},
			encoder_pulses_per_revolution=560/4,
			encoder_to_wheel_ratio=1.0,
			encoder_reversed_wheel=0,
			max_speed=8.91*Feet2Meters,	
			accel=10.0,						--We may indeed have a two button solution (match with max accel)
			brake=10.0,
			max_accel_forward=Drive_MaxAccel,			--These are in radians, just go with what feels right
			max_accel_reverse=Drive_MaxAccel,
			using_range=0,	--Warning Only use range if we have a potentiometer!
			inv_max_accel = 1/15.0,  --solved empirically
			use_aggressive_stop = 'yes'
		},

		wheel_fl =
		{
			--show_pid_dump='y',
			--is_closed=0,
			voltage_multiply=-1.0,			--reversed
			encoder_to_wheel_ratio=-1.0,  --simulation can't use encoder_reversed_wheel
		},
		wheel_fr =
		{
			--is_closed=1,
		},
		wheel_rl =
		{
			--is_closed=1,
			voltage_multiply=-1.0,			--reversed
			encoder_to_wheel_ratio=-1.0,
		},
		wheel_rr =
		{
			--is_closed=1,
		},
		
		swivel_common =
		{
			is_closed=0,
			ds_display_row=-1,
			use_pid_up_only='y',
			pid_up={p=100, i=0, d=25},
			pid_down={p=100, i=0, d=25},
			tolerance=0.03,
			tolerance_count=1,
			voltage_multiply=1.0,			--May be reversed
			--this may be 184: 84 * 36 : 20... using 180 as the ring is 3.8571428571428571428571428571429
			encoder_to_wheel_ratio=1.0,
			max_speed=2.0,	--100 rpm... with a 12:36 reduction in radians
			accel=10.0,						--We may indeed have a two button solution (match with max accel)
			brake=10.0,
			using_range=0,	--Warning Only use range if we have a potentiometer!
			max_range_deg= 45,
			min_range_deg=-45,
			starting_position=0,
			pot_offset=-45.0 * Deg2Rad,
			use_aggressive_stop = 'yes',
		},

		swivel_fl =
		{
			is_closed=1,
			--center around 450
			pot_min_limit=200,  --45 forward   0
			pot_max_limit=762,  -- 45 counter clockwise  962
			pot_range_flipped='y',
			using_range=1,	--Warning Only use range if we have a potentiometer!
		},
		swivel_fr =
		{
			is_closed=1,
			--center around 450
			pot_min_limit=200,  --45 forward   0
			pot_max_limit=762,  -- 45 counter clockwise  962
			pot_range_flipped='y',
			using_range=1,	--Warning Only use range if we have a potentiometer!
		},
		swivel_rl =
		{
			is_closed=1,
			--center around 450
			pot_min_limit=200,  --45 forward   0
			pot_max_limit=762,  -- 45 counter clockwise  962
			pot_range_flipped='y',
			using_range=1,	--Warning Only use range if we have a potentiometer!
		},
		swivel_rr =
		{
			is_closed=1,
			--center around 450
			pot_min_limit=200,  --45 forward   0
			pot_max_limit=762,  -- 45 counter clockwise  962
			pot_range_flipped='y',
			using_range=1,	--Warning Only use range if we have a potentiometer!
		}
	},

	robot_settings =
	{
		ds_display_row=-1,					--This will display the coordinates and heading (may want to leave on)

		enable_arm_auto_position='y',
		height_presets =
		--Heights are in inches
		{rest=0.0, tote_3=11.75*2 + 2 },
		auton =
		{
			first_move_ft=2,
			side_move_rad=10,
			arm_height_in=12,
			support_hotspot='n',
			auton_test=AutonTest_DoNothing,
			--auton_test=AutonTest_TestArm,
			show_auton_variables='y'
		},

		
		arm_common =
		{
			is_closed=1,
			show_pid_dump='n',
			ds_display_row=-1,
			use_pid_up_only='y',
			pid_up=
			{p=100, i=0, d=25},
			pid_down=
			{p=100, i=0, d=25},
			tolerance=0.3,
			tolerance_count=1,
			voltage_multiply=1.0,			--May be reversed
			encoder_to_wheel_ratio=1.0,
			max_speed=6.0,
			accel=10.0,						--We may indeed have a two button solution (match with max accel)
			brake=10.0,
			max_accel_forward=5,			--just go with what feels right
			max_accel_reverse=5,
			using_range=1,					--Warning Only use range if we have a potentiometer!
			use_aggressive_stop = 'yes',
		},

		turret =
		{
			is_closed=0,
			using_range=0,
			tolerance=2 * Deg2Rad,
			max_speed=0.7,	--100 rpm... with a 15x reduction in radians
			max_accel_forward=3,			--These are in radians, just go with what feels right
			max_accel_reverse=3,
			using_range=1,	--Warning Only use range if we have a potentiometer!

			max_range_deg= 180,
			min_range_deg=-180,
			starting_position=0,
			--inv_max_accel_up = 0.3,
			--inv_max_decel_up = 0.3,
			--inv_max_accel_down = 0.3,
			--inv_max_decel_down = 0.3,
		},
		arm =
		{
			pot_min_limit=232,
			pot_max_limit=890,
			pot_range_flipped='n',
			predict_up=.200,
			predict_down=.200,
			--These min/max are in inch units
			max_range= 10,
			min_range=1,
			pot_offset=1,
			starting_position=6,
			forward_deadzone=0.23,
			reverse_deadzone=0.23,
		},
		boom =
		{
			tolerance=0.15,
			pot_min_limit=200,
			pot_max_limit=834,
			pot_range_flipped='n',
			max_accel_forward=25,			--just go with what feels right
			max_accel_reverse=25,
			predict_up=.200,
			predict_down=.200,
			--These min/max are in inch units
			max_range= 10,
			min_range=1,
			pot_offset=1,
			starting_position=6,
			forward_deadzone=0.37,
			reverse_deadzone=0.37,
		},
		bucket =
		{
			tolerance=0.0125,
			pot_min_limit=290,
			pot_max_limit=888,
			pot_range_flipped='y',
			--will need to compute proper was 0.64 but seems too slow
			max_speed=3,	
			max_accel_forward=500,			--just go with what feels right
			max_accel_reverse=500,
			--These min/max are in inch units
			max_range= 12,
			min_range=0.0,
			starting_position=6,
		},
		clasp =
		{
			--show_pid_dump='y',
			tolerance=0.0125,
			pot_min_limit=440,
			pot_max_limit=760,
			pot_range_flipped='y',
			
			max_speed=3,
			max_speed_forward=1.5,
			max_speed_reverse=-1.5,	
			max_accel_forward=500,			--just go with what feels right
			max_accel_reverse=500,
			--These min/max are in inch units
			max_range= 7,
			min_range=0,
			starting_position=3.5,
			
			voltage_stall_safety_error_threshold=0.40,
			voltage_stall_safety_off_time=0.200,
			voltage_stall_safety_on_time=0.05,
			voltage_stall_safety_on_burst_level=0.75,
			voltage_stall_safety_burst_timeout=10,
			voltage_stall_safety_stall_count=40
		},
		arm_pos_common =
		{
			--is_closed=0,
			show_pid_dump='n',
			ds_display_row=-1,
			use_pid_up_only='y',
			pid_up=
			{p=100, i=0, d=25},
			pid_down=
			{p=100, i=0, d=25},
			tolerance=0.6,
			tolerance_count=20,
			voltage_multiply=1.0,
			encoder_to_wheel_ratio=1.0,
			max_speed=6.0,	--inches per second
			accel=10.0,
			brake=10.0,
			max_accel_forward=10000,			--just go with what feels right <--god mode
			max_accel_reverse=10000,
			using_range=0,					--some use range
			use_aggressive_stop = 'n',
		},

		arm_xpos =
		{
			--is_closed=0,
			pot_min_limit=12.79,  --inches from big arm's pivot point on base mount
			pot_max_limit=55.29,
			pot_range_flipped='n',
			using_range=0,	
			--These min/max are in inch units
			max_range= 55.29,
			min_range=12.79,
			starting_position=32.801521314123598,  --mathematically ideal for middle of LA... good to test code, but not necessarily for actual use
		},
		arm_ypos =
		{
			--is_closed=0,
			pid_up=
			{p=0, i=0, d=0},
			pid_down=
			{p=0, i=0, d=0},
			pot_min_limit=-20.0,  --inches from big arm's pivot point on base mount
			pot_max_limit=40.0,
			pot_range_flipped='n',
			using_range=1,					--not sure why this is using a range
			--These min/max are in inch units
			max_range= 40.0,
			min_range=-20.0,
			starting_position=-0.97606122071131374,  --mathematically ideal for middle of LA... good to test code, but not necessarily for actual use
			use_aggressive_stop = 'y',
		},
		bucket_angle =
		{
			--is_closed=0,
			--0 holding, 90 gripping, 180 dropping
			pot_min_limit=0.0,  --Degrees from horizontal (from outward side)  can go a bit less but not practical
			pot_max_limit=180.0,  --can actually go to 196
			pot_range_flipped='n',
			
			max_speed=36.66,	--degrees per second  (matches 0.64 in radians)
			accel=50.0,
			brake=50.0,
			--These min/max are in inch units
			max_range= 180.0,
			min_range=0.0,
			starting_position=78.070524788111342,  --mathematically ideal for middle of LA... good to test code, but not necessarily for actual use
		},
		clasp_angle =
		{
			--0 holding, 90 gripping, 180 dropping
			pot_min_limit=-7.0,  --Degrees goes inside slightly when negative
			pot_max_limit=100.0,  --can actually go to 106 or more
			pot_range_flipped='n',
			
			max_speed=36.66,	--degrees per second  (matches 0.64 in radians)
			accel=5.0,
			brake=5.0,
			using_range=1,					--also using range
			--These min/max are in inch units
			max_range= 100.0,
			min_range=-7.0,
			starting_position=13.19097419,  --mathematically ideal for middle of LA... good to test code, but not necessarily for actual use
		},
		wheel_cl =
		{
			is_closed=EncoderLoop,
			show_pid_dump='n',
			ds_display_row=-1,
			pid={p=200, i=0, d=25},
			voltage_multiply=1.0,			--May be reversed
			--Note: this is only used in simulation as 884 victors were phased out, but encoder simulators still use it
			curve_voltage=
			{t4=3.1199, t3=-4.4664, t2=2.2378, t1=0.1222, c=0},
			--this may be 184: 84 * 36 : 20... using 180 as the ring is 3.8571428571428571428571428571429
			encoder_to_wheel_ratio=1.0,
			encoder_pulses_per_revolution=560,
			--Arm_SetPotentiometerSafety=true,	
			max_speed=8.91*Feet2Meters,	
			accel=10.0,						--We may indeed have a two button solution (match with max accel)
			brake=10.0,
			max_accel_forward=Drive_MaxAccel,			--These are in radians, just go with what feels right
			max_accel_reverse=Drive_MaxAccel,
			using_range=0,	--Warning Only use range if we have a potentiometer!
			inv_max_accel = 1/15.0,  --solved empirically
			use_aggressive_stop = 'yes'
		},
		wheel_cr =
		{
			is_closed=EncoderLoop,
			show_pid_dump='n',
			ds_display_row=-1,
			pid={p=200, i=0, d=25},
			voltage_multiply=1.0,			--May be reversed
			--Note: this is only used in simulation as 884 victors were phased out, but encoder simulators still use it
			curve_voltage=
			{t4=3.1199, t3=-4.4664, t2=2.2378, t1=0.1222, c=0},
			--this may be 184: 84 * 36 : 20... using 180 as the ring is 3.8571428571428571428571428571429
			encoder_to_wheel_ratio=1.0,
			encoder_pulses_per_revolution=560,
			encoder_reversed_wheel='y',
			--Arm_SetPotentiometerSafety=true,	
			max_speed=8.91*Feet2Meters,	--100 rpm... with a 12:36 reduction in radians
			accel=10.0,						--We may indeed have a two button solution (match with max accel)
			brake=10.0,
			max_accel_forward=Drive_MaxAccel,			--These are in radians, just go with what feels right
			max_accel_reverse=Drive_MaxAccel,
			using_range=0,	--Warning Only use range if we have a potentiometer!
			inv_max_accel = 1/15.0,  --solved empirically
			use_aggressive_stop = 'yes'
		},
	},

	controls =
	{
		slotlist = {slot_1="airflo"},
		--field_centric_x_axis_threshold=0.40,
		--tank_steering_tolerance=0.05,
		Joystick_1 =
		{
			control = "airflo",
			axis_count = 4,
			Analog_Turn = {type="joystick_analog", key=0, is_flipped=false, multiplier=1.0, filter=0.3, curve_intensity=1.0},
			--Analog_Turn = {type="joystick_culver", key_x=5, key_y=2, is_flipped=false, multiplier=1.0, filter=0.3, curve_intensity=1.0},
			Joystick_SetCurrentSpeed_2 = {type="joystick_analog", key=2, is_flipped=true, multiplier=1.0, filter=0.1, curve_intensity=0.0},
			--Joystick_FieldCentric_XAxis = {type="joystick_analog", key=0, is_flipped=false, multiplier=1.0, filter=0.3, curve_intensity=1.0},
			--Joystick_FieldCentric_YAxis = {type="joystick_analog", key=1, is_flipped=true, multiplier=1.0, filter=0.1, curve_intensity=0.0},
			--FieldCentric_Enable = {type="joystick_button", key=4, on_off=false},
			--Robot_SetDriverOverride = {type="joystick_button", key=5, on_off=true},
			--scaled down to 0.5 to allow fine tuning and a good top acceleration speed (may change with the lua script tweaks)
			--Turret_SetCurrentVelocity = {type="joystick_analog", key=2, is_flipped=false, multiplier=0.75, filter=0.3, curve_intensity=3.0},
			--PitchRamp_SetCurrentVelocity = {type="joystick_analog", key=2, is_flipped=true, multiplier=1.0, filter=0.1, curve_intensity=2.0},
			POV_Turn =  {type="joystick_analog", key=8, is_flipped=false, multiplier=1.0, filter=0.0, curve_intensity=0.0},
			--Turn_180 = {type="joystick_button", key=7, on_off=false},
			--Turn_180_Hold = {type="joystick_button", key=7, on_off=true},
			--FlipY_Hold = {type="joystick_button", key=7, on_off=true},
			--SlideHold = {type="joystick_button", key=7, on_off=true},
			--TestWaypoint={type="joystick_button", key=3, keyboard='q', on_off=true},
			
			Robot_BallTargeting_On={type="keyboard", key='t', on_off=false},
			Robot_BallTargeting_Off={type="keyboard", key='y', on_off=false},
			TestAuton={type="keyboard", key='g', on_off=false},
			--Slide={type="keyboard", key='g', on_off=false},
			
			turret_SetCurrentVelocity = {type="joystick_analog", key=1, is_flipped=true, multiplier=1.0, filter=0.1, curve_intensity=3.0},
			--arm_SetCurrentVelocity = {type="joystick_analog", key=2, is_flipped=true, multiplier=1.0, filter=0.1, curve_intensity=3.0},
			bucket_angle_Advance={type="keyboard", key='i', on_off=true},
			bucket_angle_Retract={type="keyboard", key='u', on_off=true},
			arm_xpos_Advance={type="keyboard", key='k', on_off=true},
			arm_xpos_Retract={type="keyboard", key='j', on_off=true},
			arm_ypos_Advance={type="keyboard", key=';', on_off=true},
			arm_ypos_Retract={type="keyboard", key='l', on_off=true},
			clasp_angle_Advance={type="keyboard", key='o', on_off=true},
			clasp_angle_Retract={type="keyboard", key='p', on_off=true},
			StopAuton={type="keyboard", key='x', on_off=true},
		},
		
		Joystick_2 =
		{
			control = "logitech dual action",
			--Joystick_SetLeftVelocity = {type="joystick_analog", key=1, is_flipped=true, multiplier=1.0, filter=0.1, curve_intensity=3.0},
			--Joystick_SetLeft_XAxis = {type="joystick_analog", key=0, is_flipped=false, multiplier=1.0, filter=0.1, curve_intensity=1.0},
			--Joystick_SetRightVelocity = {type="joystick_analog", key=5, is_flipped=true, multiplier=1.0, filter=0.1, curve_intensity=3.0},
			--Joystick_SetRight_XAxis = {type="joystick_analog", key=2, is_flipped=false, multiplier=1.0, filter=0.1, curve_intensity=1.0},
			Analog_Turn = {type="joystick_analog", key=2, is_flipped=false, multiplier=1.0, filter=0.3, curve_intensity=1.0},
			--Analog_Turn = {type="joystick_culver", key_x=2, key_y=5, is_flipped=false, multiplier=1.0, filter=0.3, curve_intensity=1.0},
			--Joystick_SetCurrentSpeed_2 = {type="joystick_analog", key=1, is_flipped=true, multiplier=1.0, filter=0.1, curve_intensity=0.0},
			Joystick_FieldCentric_XAxis = {type="joystick_analog", key=0, is_flipped=false, multiplier=1.0, filter=0.3, curve_intensity=1.0},
			Joystick_FieldCentric_YAxis = {type="joystick_analog", key=1, is_flipped=true, multiplier=1.0, filter=0.1, curve_intensity=0.0},
			--Turret_SetCurrentVelocity = {type="joystick_analog", key=2, is_flipped=false, multiplier=0.5, filter=0.1, curve_intensity=0.0},
			Robot_SetLowGearOff = {type="joystick_button", key=6, on_off=false},
			Robot_SetLowGearOn = {type="joystick_button", key=5, on_off=false},
			
			--Ball_Squirt = {type="joystick_button", key=1, on_off=true},
			--PowerWheels_IsRunning = {type="joystick_button", key=7, on_off=true},
			POV_Turn =  {type="joystick_analog", key=8, is_flipped=false, multiplier=1.0, filter=0.0, curve_intensity=0.0},
			Turn_180_Hold = {type="joystick_button", key=7, on_off=true},
			FlipY_Hold = {type="joystick_button", key=7, on_off=true},
			SlideHold = {type="joystick_button", key=7, on_off=true}
		},
		Joystick_3 =
		{
			control = "gamepad f310 (controller)",
			Analog_Turn = {type="joystick_analog", key=0, is_flipped=false, multiplier=1.0, filter=0.3, curve_intensity=1.0},
			--Analog_Turn = {type="joystick_culver", key_x=3, key_y=4, is_flipped=false, multiplier=1.0, filter=0.3, curve_intensity=1.0},
			Joystick_SetCurrentSpeed_2 = {type="joystick_analog", key=1, is_flipped=true, multiplier=1.0, filter=0.1, curve_intensity=0.0},
			--Joystick_SetLeftVelocity = {type="joystick_analog", key=1, is_flipped=true, multiplier=1.0, filter=0.1, curve_intensity=3.0},
			--Joystick_SetRightVelocity = {type="joystick_analog", key=4, is_flipped=true, multiplier=1.0, filter=0.1, curve_intensity=3.0},
			--Joystick_FieldCentric_XAxis = {type="joystick_analog", key=0, is_flipped=false, multiplier=1.0, filter=0.3, curve_intensity=1.0},
			--Joystick_FieldCentric_YAxis = {type="joystick_analog", key=1, is_flipped=true, multiplier=1.0, filter=0.1, curve_intensity=0.0},
			--Turret_SetCurrentVelocity = {type="joystick_analog", key=3, is_flipped=false, multiplier=0.75, filter=0.3, curve_intensity=3.0},
			--PitchRamp_SetCurrentVelocity = {type="joystick_analog", key=4, is_flipped=true, multiplier=1.0, filter=0.1, curve_intensity=2.0},
			
			arm_SetCurrentVelocity = {type="joystick_analog", key=4, is_flipped=true, multiplier=0.6, filter=0.1, curve_intensity=3.0},
			--FieldCentric_EnableValue = {type="joystick_analog", key=2, is_flipped=true, multiplier=1.0, filter=0.1, curve_intensity=0.0},

			Robot_SetLowGearOff = {type="joystick_button", key=6, on_off=false},
			Robot_SetLowGearOn = {type="joystick_button", key=5, on_off=false},
			TestWaypoint={type="joystick_button", key=3, on_off=true},
		},
		Joystick_4 =
		{
			control = "controller (xbox 360 for windows)",
			--Joystick_SetLeft_XAxis = {type="joystick_analog", key=0, is_flipped=false, multiplier=1.0, filter=0.1, curve_intensity=1.0},
			--Joystick_SetRight_XAxis = {type="joystick_analog", key=2, is_flipped=false, multiplier=1.0, filter=0.1, curve_intensity=1.0},
			--Analog_Turn = {type="joystick_analog", key=0, is_flipped=false, multiplier=1.0, filter=0.3, curve_intensity=1.0},
			Joystick_FieldCentric_XAxis = {type="joystick_analog", key=0, is_flipped=false, multiplier=1.0, filter=0.3, curve_intensity=1.0},
			Joystick_FieldCentric_YAxis = {type="joystick_analog", key=1, is_flipped=true, multiplier=1.0, filter=0.1, curve_intensity=0.0},
			Analog_Turn = {type="joystick_culver", key_x=3, key_y=4, is_flipped=false, multiplier=1.0, filter=0.3, curve_intensity=1.0},
			--Joystick_SetCurrentSpeed_2 = {type="joystick_analog", key=1, is_flipped=true, multiplier=1.0, filter=0.1, curve_intensity=0.0},
			Robot_SetLowGearOff = {type="joystick_button", key=2, on_off=false},
			Robot_SetLowGearOn = {type="joystick_button", key=1, on_off=false},
						
			POV_Turn =  {type="joystick_analog", key=8, is_flipped=false, multiplier=1.0, filter=0.0, curve_intensity=0.0},
			Robot_SetDriverOverride = {type="joystick_button", key=5, on_off=true},
			Turn_180_Hold = {type="joystick_button", key=6, on_off=true},
			FlipY_Hold = {type="joystick_button", key=6, on_off=true},
			SlideHold = {type="joystick_button", key=6, on_off=true}
		},
		Joystick_5 =
		{	
			control = "ch throttle quadrant",
			--arm_xpos_SetIntendedPosition = {type="joystick_analog", key=0, is_flipped=false, multiplier=1.0, filter=0.0, curve_intensity=0.0},
			--arm_ypos_SetIntendedPosition = {type="joystick_analog", key=1, is_flipped=false, multiplier=1.0, filter=0.0, curve_intensity=0.0},
			--bucket_angle_SetIntendedPosition = {type="joystick_analog", key=2, is_flipped=false, multiplier=1.0, filter=0.0, curve_intensity=0.0},
			--clasp_angle_SetIntendedPosition = {type="joystick_analog", key=3, is_flipped=false, multiplier=1.0, filter=0.0, curve_intensity=0.0},
			--Robot_SetDefensiveKeyValue = {type="joystick_analog", key=4, is_flipped=true, multiplier=1.0, filter=0.0, curve_intensity=0.0},

			--intermediate closed loop test point of each position control
			--turret_SetIntendedPosition = {type="joystick_analog", key=0, is_flipped=false, multiplier=1.0, filter=0.0, curve_intensity=0.0},
			--arm_SetIntendedPosition = {type="joystick_analog", key=1, is_flipped=false, multiplier=1.0, filter=0.0, curve_intensity=0.0},
			--boom_SetIntendedPosition = {type="joystick_analog", key=2, is_flipped=false, multiplier=1.0, filter=0.0, curve_intensity=0.0},
			--bucket_SetIntendedPosition = {type="joystick_analog", key=3, is_flipped=false, multiplier=1.0, filter=0.0, curve_intensity=0.0},
			--clasp_SetIntendedPosition = {type="joystick_analog", key=4, is_flipped=false, multiplier=1.0, filter=0.0, curve_intensity=0.0},

		},

	},
	
	--This is only used in the AI tester, can be ignored
	UI =
	{
		Length=5, Width=5,
		TextImage="(   )\n|   |\n(-+-)\n|   |\n(   )"
	}
}

Curivator = MainRobot
