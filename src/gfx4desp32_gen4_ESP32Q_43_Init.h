// NV3041 Init codes 4.3 QSPI
uint8_t Init_CommandNV[320] = {
	0x00, 0x38,
	0x01, 0xff, 0xa5,
	0x01, 0xe7, 0x10,
	0x01, 0x35, 0x00,
	0x01, 0x36, 0xc0,
	0x01, 0x3A, 0x01, // 01---565，00---666
	0x01, 0x40, 0x01, // 01:IPS/00:TN
	0x01, 0x41, 0x01, // 01--8bit
	0x01, 0x44, 0x15, 
	0x01, 0x45, 0x15,
	0x01, 0x7d, 0x02,
	0x01, 0xc1, 0xbb,
	0x01, 0xc2, 0x05,
	0x01, 0xc3, 0x10,
	0x01, 0xc6, 0x3e,
	0x01, 0xc7, 0x25,
	0x01, 0xc8, 0x11,
	0x01, 0x7a, 0x5f,
	0x01, 0x6f, 0x44,
	0x01, 0x78, 0x70,
	0x01, 0xc9, 0x00,
	0x01, 0x67, 0x21,
	0x01, 0x51, 0x0a,
	0x01, 0x52, 0x76,
	0x01, 0x53, 0x0a,
	0x01, 0x54, 0x76,
	0x01, 0x46, 0x0a,
	0x01, 0x47, 0x2a,
	0x01, 0x48, 0x0a,
	0x01, 0x49, 0x1a,
	0x01, 0x56, 0x43,
	0x01, 0x57, 0x42,
	0x01, 0x58, 0x3c,
	0x01, 0x59, 0x64,
	0x01, 0x5a, 0x41,
	0x01, 0x5b, 0x3c,
	0x01, 0x5c, 0x02,
	0x01, 0x5d, 0x3c,
	0x01, 0x5e, 0x1f,
	0x01, 0x60, 0x80,
	0x01, 0x61, 0x3f,
	0x01, 0x62, 0x21,
	0x01, 0x63, 0x07,
	0x01, 0x64, 0xe0,
	0x01, 0x65, 0x02,
	0x01, 0xca, 0x20,
	0x01, 0xcb, 0x52,
	0x01, 0xcc, 0x10,
	0x01, 0xcd, 0x42,
	0x01, 0xd0, 0x20,
	0x01, 0xd1, 0x52,
	0x01, 0xd2, 0x10,
	0x01, 0xd3, 0x42,
	0x01, 0xd4, 0x0a,
	0x01, 0xd5, 0x32,
    ///test  mode
	0x01, 0xf8, 0x03,
	0x01, 0xf9, 0x20,
	0x01, 0x80, 0x00, 0x01, 0xa0, 0x00,
	0x01, 0x81, 0x07, 0x01, 0xa1, 0x06,
	0x01, 0x82, 0x02, 0x01, 0xa2, 0x01,
	0x01, 0x86, 0x11, 0x01, 0xa6, 0x10,
	0x01, 0x87, 0x27, 0x01, 0xa7, 0x27,
	0x01, 0x83, 0x37, 0x01, 0xa3, 0x37,
	0x01, 0x84, 0x35, 0x01, 0xa4, 0x35,
	0x01, 0x85, 0x3f, 0x01, 0xa5, 0x3f,
	0x01, 0x88, 0x0b, 0x01, 0xa8, 0x0b,
	0x01, 0x89, 0x14, 0x01, 0xa9, 0x14,
	0x01, 0x8a, 0x1a, 0x01, 0xaa, 0x1a,
	0x01, 0x8b, 0x0a, 0x01, 0xab, 0x0a,
	0x01, 0x8c, 0x14, 0x01, 0xac, 0x08,
	0x01, 0x8d, 0x17, 0x01, 0xad, 0x07,
	0x01, 0x8e, 0x16, 0x01, 0xae, 0x06,
	0x01, 0x8f, 0x1b, 0x01, 0xaf, 0x07,
	0x01, 0x90, 0x04, 0x01, 0xb0, 0x04,
	0x01, 0x91, 0x0a, 0x01, 0xb1, 0x0a,
	0x01, 0x92, 0x16, 0x01, 0xb2, 0x15,
	0x01, 0xff, 0x00,
	0x01, 0x11, 0x00,
	// delay 700ms
	0xfe, 70,
	0x01, 0x29, 0x00,
	// delay 100ms
	0xfe, 10,
	0xff
};

#define GFX4dESP32_NV3041_TE_OUTPUT_EN					0xE7
#define GFX4dESP32_NV3041_TE_INTERFACE_EN				0x35




//NV3041_SPI_Write_cmd(0xff);
//NV3041_SPI_Write_data(0xa5);

//NV3041_SPI_Write_cmd(0xE7);	   //TE_output_en
//NV3041_SPI_Write_data(0x10);

//NV3041_SPI_Write_cmd(0x35);	   //TE_ interface_en
//NV3041_SPI_Write_data(0x00);//01

//NV3041_SPI_Write_cmd(0x36);	   //
//NV3041_SPI_Write_data(0xc0);

//NV3041_SPI_Write_cmd(0x3A);	   //
//NV3041_SPI_Write_data(0x01);//01---565，00---666

//NV3041_SPI_Write_cmd(0x40);	   
//NV3041_SPI_Write_data(0x01);//01:IPS/00:TN

//NV3041_SPI_Write_cmd(0x41);	   
//NV3041_SPI_Write_data(0x03);//01--8bit

//NV3041_SPI_Write_cmd(0x44);	   //VBP  ?????
//NV3041_SPI_Write_data(0x15);   //21

//NV3041_SPI_Write_cmd(0x45);	   //VFP  ?????
//NV3041_SPI_Write_data(0x15);   //21

//NV3041_SPI_Write_cmd(0x7d);//vdds_trim[2:0]
//NV3041_SPI_Write_data(0x03);
							  
//NV3041_SPI_Write_cmd(0xc1);//avdd_clp_en avdd_clp[1:0] avcl_clp_en avcl_clp[1:0]
//NV3041_SPI_Write_data(0xbb);//0xbb	 88		  a2

//NV3041_SPI_Write_cmd(0xc2);//vgl_clp_en vgl_clp[2:0]		
//NV3041_SPI_Write_data(0x05);

//NV3041_SPI_Write_cmd(0xc3);//vgl_clp_en vgl_clp[2:0]		
//NV3041_SPI_Write_data(0x10);

//NV3041_SPI_Write_cmd(0xc6);//avdd_ratio_sel avcl_ratio_sel vgh_ratio_sel[1:0] vgl_ratio_sel[1:0]	
//NV3041_SPI_Write_data(0x3e);  // 35

//NV3041_SPI_Write_cmd(0xc7);//mv_clk_sel[1:0] avdd_clk_sel[1:0] avcl_clk_sel[1:0]	
//NV3041_SPI_Write_data(0x25);  //2e


//NV3041_SPI_Write_cmd(0xc8);//	VGL_CLK_sel
//NV3041_SPI_Write_data(0x11); //


//NV3041_SPI_Write_cmd(0x7a);//	user_vgsp
//NV3041_SPI_Write_data(0x5f);   //4f:0.8V		3f:1.04V	5f

//NV3041_SPI_Write_cmd(0x6f);//	user_gvdd
//NV3041_SPI_Write_data(0x44);	//1C:5.61	  5f	 53		   2a	    3a

//NV3041_SPI_Write_cmd(0x78);//	user_gvcl								
//NV3041_SPI_Write_data(0x70); 	//50:-3.22	  75			58	     	66

//NV3041_SPI_Write_cmd(0xc9);//
//NV3041_SPI_Write_data(0x00); 
	

//NV3041_SPI_Write_cmd(0x67);	   //
//NV3041_SPI_Write_data(0x21);


//gate_ed

//NV3041_SPI_Write_cmd(0x51);//gate_st_o[7:0]
//NV3041_SPI_Write_data(0x0a);

//NV3041_SPI_Write_cmd(0x52);//gate_ed_o[7:0]
//NV3041_SPI_Write_data(0x76);   //76

//NV3041_SPI_Write_cmd(0x53);//gate_st_e[7:0]
//NV3041_SPI_Write_data(0x0a);	//76

//NV3041_SPI_Write_cmd(0x54);//gate_ed_e[7:0]
//NV3041_SPI_Write_data(0x76);
////sorce 
//NV3041_SPI_Write_cmd(0x46);//fsm_hbp_o[5:0]
//NV3041_SPI_Write_data(0x0a);

//NV3041_SPI_Write_cmd(0x47);//fsm_hfp_o[5:0]
//NV3041_SPI_Write_data(0x2a);

//NV3041_SPI_Write_cmd(0x48);//fsm_hbp_e[5:0]
//NV3041_SPI_Write_data(0x0a);

//NV3041_SPI_Write_cmd(0x49);//fsm_hfp_e[5:0]
//NV3041_SPI_Write_data(0x1a);
//NV3041_SPI_Write_cmd(0x56);//src_ld_wd[1:0] src_ld_st[5:0]
//NV3041_SPI_Write_data(0x43);

//NV3041_SPI_Write_cmd(0x57);//pn_cs_en src_cs_st[5:0]
//NV3041_SPI_Write_data(0x42);

//NV3041_SPI_Write_cmd(0x58);//src_cs_p_wd[6:0]
//NV3041_SPI_Write_data(0x3c);

//NV3041_SPI_Write_cmd(0x59);//src_cs_n_wd[6:0]
//NV3041_SPI_Write_data(0x64); 

//NV3041_SPI_Write_cmd(0x5a);//src_pchg_st_o[6:0]
//NV3041_SPI_Write_data(0x41);  //41

//NV3041_SPI_Write_cmd(0x5b);//src_pchg_wd_o[6:0]
//NV3041_SPI_Write_data(0x3c); 

//NV3041_SPI_Write_cmd(0x5c);//src_pchg_st_e[6:0]
//NV3041_SPI_Write_data(0x02);	//02

//NV3041_SPI_Write_cmd(0x5d);//src_pchg_wd_e[6:0]
//NV3041_SPI_Write_data(0x3c);	//3c


//NV3041_SPI_Write_cmd(0x5e);//src_pol_sw[7:0]
//NV3041_SPI_Write_data(0x1f);
//NV3041_SPI_Write_cmd(0x60);//src_op_st_o[7:0]
//NV3041_SPI_Write_data(0x80);

//NV3041_SPI_Write_cmd(0x61);//src_op_st_e[7:0]
//NV3041_SPI_Write_data(0x3f);

//NV3041_SPI_Write_cmd(0x62);//src_op_ed_o[9:8] src_op_ed_e[9:8]
//NV3041_SPI_Write_data(0x21);

//NV3041_SPI_Write_cmd(0x63);//src_op_ed_o[7:0]
//NV3041_SPI_Write_data(0x07);

//NV3041_SPI_Write_cmd(0x64);//src_op_ed_e[7:0]
//NV3041_SPI_Write_data(0xe0);

//NV3041_SPI_Write_cmd(0x65);//chopper
//NV3041_SPI_Write_data(0x02);

//NV3041_SPI_Write_cmd(0xca);	   //avdd_mux_st_o[7:0]
//NV3041_SPI_Write_data(0x20);

//NV3041_SPI_Write_cmd(0xcb);	   //avdd_mux_ed_o[7:0]
//NV3041_SPI_Write_data(0x52);	 //52

//NV3041_SPI_Write_cmd(0xcc);	   //avdd_mux_st_e[7:0]
//NV3041_SPI_Write_data(0x10);

//NV3041_SPI_Write_cmd(0xcD);	   //avdd_mux_ed_e[7:0]
//NV3041_SPI_Write_data(0x42);

//NV3041_SPI_Write_cmd(0xD0);	   //avcl_mux_st_o[7:0]
//NV3041_SPI_Write_data(0x20);

//NV3041_SPI_Write_cmd(0xD1);	   //avcl_mux_ed_o[7:0]
//NV3041_SPI_Write_data(0x52);

//NV3041_SPI_Write_cmd(0xD2);	   //avcl_mux_st_e[7:0]
//NV3041_SPI_Write_data(0x10);

//NV3041_SPI_Write_cmd(0xD3);	   //avcl_mux_ed_e[7:0]
//NV3041_SPI_Write_data(0x42);

//NV3041_SPI_Write_cmd(0xD4);	   //vgh_mux_st[7:0]
//NV3041_SPI_Write_data(0x0a);

//NV3041_SPI_Write_cmd(0xD5);	   //vgh_mux_ed[7:0]
//NV3041_SPI_Write_data(0x32);

///test  mode
//NV3041_SPI_Write_cmd(0xf8);	   //
//NV3041_SPI_Write_data(0x03);	//16????    10-15??????

//NV3041_SPI_Write_cmd(0xf9);	   //	  ??????
//NV3041_SPI_Write_data(0x20);

//BOE4.3IPS-1
////gammma   0930
//NV3041_SPI_Write_cmd(0x80);	   //gam_vrp0	    
//NV3041_SPI_Write_data(0x00);   //00
//NV3041_SPI_Write_cmd(0xA0);	   //gam_VRN0		  
//NV3041_SPI_Write_data(0x00);   //00

//NV3041_SPI_Write_cmd(0x81);	   //gam_vrp1	    
//NV3041_SPI_Write_data(0x07);   //06
//NV3041_SPI_Write_cmd(0xA1);	   //gam_VRN1		  
//NV3041_SPI_Write_data(0x06);   //06

//NV3041_SPI_Write_cmd(0x82);	   //gam_vrp2	    
//NV3041_SPI_Write_data(0x02);   //02
//NV3041_SPI_Write_cmd(0xA2);	   //gam_VRN2		  
//NV3041_SPI_Write_data(0x01);   //02

//NV3041_SPI_Write_cmd(0x86);	   //gam_prp0	 	  
//NV3041_SPI_Write_data(0x11);	 //0E
//NV3041_SPI_Write_cmd(0xA6);	   //gam_PRN0	 	  
//NV3041_SPI_Write_data(0x10);	 //0c

//NV3041_SPI_Write_cmd(0x87);	   //gam_prp1	   	
//NV3041_SPI_Write_data(0x27);   //1c
//NV3041_SPI_Write_cmd(0xA7);	   //gam_PRN1	 	   
//NV3041_SPI_Write_data(0x27);   //1c

//NV3041_SPI_Write_cmd(0x83);	   //gam_vrp3	    
//NV3041_SPI_Write_data(0x37);   //33
//NV3041_SPI_Write_cmd(0xA3);	   //gam_VRN3		  
//NV3041_SPI_Write_data(0x37);   //33

//NV3041_SPI_Write_cmd(0x84);	   //gam_vrp4	    
//NV3041_SPI_Write_data(0x35);   //24
//NV3041_SPI_Write_cmd(0xA4);	   //gam_VRN4		  
//NV3041_SPI_Write_data(0x35);   //24

//NV3041_SPI_Write_cmd(0x85);	   //gam_vrp5	    
//NV3041_SPI_Write_data(0x3f);   //3f
//NV3041_SPI_Write_cmd(0xA5);	   //gam_VRN5		  
//NV3041_SPI_Write_data(0x3f);   //3f
//

//NV3041_SPI_Write_cmd(0x88);	   //gam_pkp0	    
//NV3041_SPI_Write_data(0x0b);	  //0b
//NV3041_SPI_Write_cmd(0xA8);	   //gam_PKN0		  
//NV3041_SPI_Write_data(0x0b);	  //0b

//NV3041_SPI_Write_cmd(0x89);	   //gam_pkp1	    
//NV3041_SPI_Write_data(0x14);   //14
//NV3041_SPI_Write_cmd(0xA9);	   //gam_PKN1		  
//NV3041_SPI_Write_data(0x14);   //14

	

//NV3041_SPI_Write_cmd(0x8a);	   //gam_pkp2	    
//NV3041_SPI_Write_data(0x1a);	//1a
//NV3041_SPI_Write_cmd(0xAa);	   //gam_PKN2		  
//NV3041_SPI_Write_data(0x1a);	//1a

//NV3041_SPI_Write_cmd(0x8b);	   //gam_PKP3	    
//NV3041_SPI_Write_data(0x0a);   //0a
//NV3041_SPI_Write_cmd(0xAb);	   //gam_PKN3		  
//NV3041_SPI_Write_data(0x0a);   //0a

//NV3041_SPI_Write_cmd(0x8c);	   //gam_PKP4	    
//NV3041_SPI_Write_data(0x14);   //14
//NV3041_SPI_Write_cmd(0xAc);	   //gam_PKN4		  
//NV3041_SPI_Write_data(0x08);   //08

//NV3041_SPI_Write_cmd(0x8d);	   //gam_PKP5		  
//NV3041_SPI_Write_data(0x17);   //17
//NV3041_SPI_Write_cmd(0xAd);	   //gam_PKN5		 
//NV3041_SPI_Write_data(0x07);   //07

//NV3041_SPI_Write_cmd(0x8e);	   //gam_PKP6		  
//NV3041_SPI_Write_data(0x16);   //16
//NV3041_SPI_Write_cmd(0xAe);	   //gam_PKN6		  
//NV3041_SPI_Write_data(0x06);   //06

//NV3041_SPI_Write_cmd(0x8f);	   //gam_PKP7		  
//NV3041_SPI_Write_data(0x1b);   //1b
//NV3041_SPI_Write_cmd(0xAf);	   //gam_PKN7		  
//NV3041_SPI_Write_data(0x07);   //07

//NV3041_SPI_Write_cmd(0x90);	   //gam_PKP8		  
//NV3041_SPI_Write_data(0x04);   //04
//NV3041_SPI_Write_cmd(0xB0);	   //gam_PKN8		  
//NV3041_SPI_Write_data(0x04);   //04

//NV3041_SPI_Write_cmd(0x91);	   //gam_PKP9		  
//NV3041_SPI_Write_data(0x0a);   //0a
//NV3041_SPI_Write_cmd(0xB1);	   //gam_PKN9		 
//NV3041_SPI_Write_data(0x0a);   //0a
												 
//NV3041_SPI_Write_cmd(0x92);	   //gam_PKP10		
//NV3041_SPI_Write_data(0x16);   //16
//NV3041_SPI_Write_cmd(0xB2);	   //gam_PKN10		
//NV3041_SPI_Write_data(0x15);   //15


//NV3041_SPI_Write_cmd(0xff);
//NV3041_SPI_Write_data(0x00);  

//NV3041_SPI_Write_cmd(0x11);
//NV3041_SPI_Write_data(0x00); 

//Delay_ms(700);
								
//NV3041_SPI_Write_cmd(0x29);
//NV3041_SPI_Write_data(0x00); 

//Delay_ms(100);