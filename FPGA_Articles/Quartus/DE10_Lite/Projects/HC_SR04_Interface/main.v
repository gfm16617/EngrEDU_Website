
`default_nettype none

module main(
	// Board I/Os
	input		MAX10_CLK1_50,
	input 	[9:0]		SW,
	output	[9:0]		LEDR,
	inout		[35:0]	GPIO,
	inout		[15:0]	ARDUINO_IO,
	output 	[7:0]		HEX0,
	output 	[7:0]		HEX1,
	output 	[7:0]		HEX2
);


	// INTERFACE WIRES
	//*********************
	// Wires to make the connection easier between main module with the counter
	wire Clock;
	wire Resetn;
	
	assign Clock = MAX10_CLK1_50;
	assign Resetn = SW[0];
	//*********************
	

	// HC-SR04 Trigger Generator
	//*********************

	trigger_generator TG0(
		.Clock(Clock),
		.Resetn(Resetn),
		//.cycle_out(GPIO[34]),
		//.trig_out(GPIO[35])
		.cycle_out(ARDUINO_IO[0]),
		.trig_out(ARDUINO_IO[11])
	);
	//*********************
	
	
	// HC-SR04 Echo Receiver
	//*********************
	
	wire [20:0] ticks;  // us
	wire w_valid;
	
	echo_ticks ET0(
		.clk(Clock),
		.rst_n(Resetn),
		//.echo_in(GPIO[32]),
		.echo_in(ARDUINO_IO[10]),
		.width_ticks(ticks),
		//.valid(LEDR[0]),
		.valid(w_valid),
		.timeout(LEDR[1]),
		.busy(LEDR[2])
	);
	
	// Distance Display
	//*********************

	//wire [20:0] ticks = 21'd29000;  // Test signal
	
	show_ticks_cm_3hex ST0(
    .clk(Clock),
    .rst_n(Resetn),         // active-low reset
    //.sample(1'b1),        // latch new ticks this cycle (use your 'valid' strobe). Tie 1'b1 for continuous.
    .sample(w_valid),
	 .ticks_20ns(ticks),    // 21-bit width in 20 ns ticks
    .HEX2(HEX2[6:0]),          // hundreds (active-low segments {g,f,e,d,c,b,a})
    .HEX1(HEX1[6:0]),          // tens
    .HEX0(HEX0[6:0])           // ones
	);
	
	// turn off the dot
	assign HEX0[7] = 1'b1;
	assign HEX1[7] = 1'b1;
	assign HEX2[7] = 1'b1;
	
	//*********************

endmodule
