
`default_nettype none

module trigger_generator(
	// Module IOs
	input 	Clock,
	input 	Resetn,
	output 	cycle_out,
	output 	trig_out
	
);
	
	// WIRES & PARAMETERS
	
	localparam DIVISOR_TRIG = 500;  // 10us
	localparam DIVISOR_CYCLE = 3000000;  // 60ms
	
	reg [21:0] counter_cycle;  // 2^22 = 4,194,304 > 3,000,000
	
	reg led_out;  // monitor 60ms cycle

	// Cycle Comparator
	//************************
	always @(posedge Clock, negedge Resetn)
		if(!Resetn)
		begin
			// reset both counters
			counter_cycle <= 0;
			led_out <= 0;
		
		end else
		begin
			if(counter_cycle == DIVISOR_CYCLE - 1)
			begin
				counter_cycle <= 0;
				led_out <= ~led_out;  // toggle LED
			
			end else
			begin
				counter_cycle <= counter_cycle + 1;
			end
			
		end
	//************************

	// Module Output
	assign cycle_out = led_out;
	
	// Trigger Comparator with register
	//************************
	reg trig_out_q;
	always @(posedge Clock)
		trig_out_q <= (counter_cycle < DIVISOR_TRIG);
	//************************
	
	// Module Output	
	assign trig_out = trig_out_q;  // output registered
	
	//assign trig_out = (counter_cycle < DIVISOR_TRIG);  // Combinationl Logic option

endmodule
