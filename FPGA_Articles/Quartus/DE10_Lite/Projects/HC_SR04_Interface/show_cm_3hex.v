//------------------------------------------------------------------------------
// Show distance (from 20 ns tick counter) on HEX2..HEX0, DE10-Lite.
// Verilog-2001, active-low segments.
// ticks_20ns is the pulse width in 20 ns units (i.e., at 50 MHz).
// Distance formula: cm ≈ ticks / 2900
// We implement cm ≈ (ticks * 46282 + 2^26) >> 27  (rounded), since 46282/2^27 ≈ 1/2900.
//------------------------------------------------------------------------------

`default_nettype none

module show_ticks_cm_3hex
#(
    parameter LEADING_BLANK = 1  // 1: blank leading zeros on HEX2/HEX1
)
(
    input             clk,
    input             rst_n,         // active-low reset
    input             sample,        // latch new ticks this cycle (use your 'valid' strobe). Tie 1'b1 for continuous.
    input      [20:0] ticks_20ns,    // 21-bit width in 20 ns ticks
    output reg [6:0]  HEX2,          // hundreds (active-low segments {g,f,e,d,c,b,a})
    output reg [6:0]  HEX1,          // tens
    output reg [6:0]  HEX0           // ones
);

    // --- Latch the newest measurement only when 'sample' is asserted
    reg [20:0] ticks_q;
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) ticks_q <= 21'd0;
        else if (sample) ticks_q <= ticks_20ns;
    end

	 
    // --- ticks -> centimeters, rounded: cm ≈ (ticks * 46282 + 2^26) >> 27
    // 46282/2^27 ≈ 1/2900 with tiny error; 21b * 16b -> <=37b; keep margin.
    function [9:0] ticks_to_cm10;  // returns up to 1023 (we'll clamp to 400)
        input [20:0] t;
        reg   [47:0] p;            // wide enough for product + rounding
        begin
            p = (t * 16'd46282) + 48'd67108864; // +2^26 for rounding
            ticks_to_cm10 = p[47:27];           // >> 27
        end
    endfunction

	wire [9:0] cm_raw = ticks_to_cm10(ticks_q);
	
	 
	 /*
	 // --- ticks (20 ns) -> centimeters, rounded to nearest
	localparam integer CM_RECIP_NUM  = 46282; // numerator
	localparam integer CM_RECIP_SH   = 27;    // shift amount
	
	wire [47:0] prod = (ticks_q * CM_RECIP_NUM) + (48'd1 << (CM_RECIP_SH-1)); // +2^26
	wire [9:0]  cm_raw = prod >> CM_RECIP_SH;  // == ticks / 2900 (rounded)
	*/


    // Clamp to 0..400 (your stated display range)
    wire [9:0] cm_clamped = (cm_raw > 10'd400) ? 10'd400 : cm_raw;

	 
    // --- Binary (0..999) -> BCD (hundreds, tens, ones)
    wire [3:0] bcd_h, bcd_t, bcd_o;
	 bin_to_bcd3 u_b2b (
        .bin (cm_clamped),  // safe: 0..400
        .h   (bcd_h),
        .t   (bcd_t),
        .o   (bcd_o)
    );
	 

    // --- Optional leading-zero blanking
    wire [6:0] seg_h = (LEADING_BLANK && (bcd_h == 4'd0)) ? 7'b1111111 : seg7(bcd_h);
    wire [6:0] seg_t = (LEADING_BLANK && (bcd_h == 4'd0) && (bcd_t == 4'd0))
                       ? 7'b1111111 : seg7(bcd_t);
    wire [6:0] seg_o = seg7(bcd_o);

    // --- Register outputs to avoid glitches on the display
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            HEX2 <= 7'b1111111;
            HEX1 <= 7'b1111111;
            HEX0 <= 7'b1111111;
        end else begin
            HEX2 <= seg_h;
            HEX1 <= seg_t;
            HEX0 <= seg_o;
        end
    end

    // 7-seg decoder (active-low)
    function [6:0] seg7;
        input [3:0] d;
        begin
            case (d)
                4'd0: seg7 = 7'b1000000;
                4'd1: seg7 = 7'b1111001;
                4'd2: seg7 = 7'b0100100;
                4'd3: seg7 = 7'b0110000;
                4'd4: seg7 = 7'b0011001;
                4'd5: seg7 = 7'b0010010;
                4'd6: seg7 = 7'b0000010;
                4'd7: seg7 = 7'b1111000;
                4'd8: seg7 = 7'b0000000;
                4'd9: seg7 = 7'b0010000;
                default: seg7 = 7'b1111111; // blank
            endcase
        end
    endfunction

endmodule

//------------------------------------------------------------------------------
// Binary (0..999) to 3 BCD digits (hundreds, tens, ones) via double-dabble.
//------------------------------------------------------------------------------
module bin_to_bcd3 (
    input      [9:0] bin,     // 0..999
    output reg [3:0] h,
    output reg [3:0] t,
    output reg [3:0] o
);
    integer i;
    // 22-bit shift: [21:18]=hundreds, [17:14]=tens, [13:10]=ones, [9:0]=bin
    reg [21:0] shift;

    always @* begin
        shift = {12'd0, bin};
        for (i = 0; i < 10; i = i + 1) begin
            if (shift[21:18] >= 4'd5) shift[21:18] = shift[21:18] + 4'd3;
            if (shift[17:14] >= 4'd5) shift[17:14] = shift[17:14] + 4'd3;
            if (shift[13:10] >= 4'd5) shift[13:10] = shift[13:10] + 4'd3;
            shift = shift << 1;
        end
        h = shift[21:18];
        t = shift[17:14];
        o = shift[13:10];
    end
endmodule
