let ui = {
	"title": "Arduino Universal Controler",
	"header": {
		"class": "mdl-layout--fixed-header",
		"drawer": {
			"title": "Arduino Universal Controler",
			"links": [
				{"name": "t6 IoT App", "icon": "api", "id": "t6iot", "class": "", "link": "https://api.internetcollaboratif.info"},
				{"name": "t6 Api doc", "icon": "integration_instructions", "id": "apidoc", "class": "", "link": "https://doc.internetcollaboratif.info"},
				{"spacer": true},
				{"name": "Digital", "icon": "pin_invoke", "id": "digitalMenu", "class": "", "link": "#digital"},
				{"name": "Analog", "icon": "timeline", "id": "analogMenu", "class": "", "link": "#analog"},
				{"name": "Led", "icon": "wb_incandescent", "id": "ledMenu", "class": "", "link": "#led"},
				{"name": "Audio", "icon": "volume_up", "id": "audioMenu", "class": "", "link": "#audio"},
				{"spacer": true}
			],
			"text": {
				"id": "credit",
				"text": "t6 IoT © 2022 - v1.0.0"
			}
		},
		"links": [
			{"name": "t6 IoT App", "id": "t6iot", "class": "", "link": "https://api.internetcollaboratif.info"},
			{"name": "t6 Api doc", "id": "apidoc", "class": "", "link": "https://doc.internetcollaboratif.info"}
		]
	},
	"tab_contents":[
		{
			"name": "Digital",
			"id": "digital",
			"class": "is-active",
			"icon": "pin_invoke",
			"width":12,
			"body": {
				"rows":[
					{
						"row_id": "tab1_row1",
						"width":12,
						"columns":[
							{
								"width":12,
								"col_id":"tab1_row1_col1",
								"cards":[
									{
										"width":6,
										"title":"Digital Write",
										"body":{
											"lists": [
												{
													"icon": "pin_invoke",
													"label":"Digital Pin D0",
													"body":"Must be HIGH during boot and LOW for programming",
													"switches":[
														{
															"id":"pin0",
															"valueUnchecked":0,
															"labelUnchecked":"LOW",
															"valueChecked":1,
															"labelChecked":"HIGH",
															"defaultState":"unchecked",
															"action":"/digitalWrite?pin=0&value=%s"
														}
													]
												},
												{
													"icon": "pin_invoke",
													"label":"Digital Pin TX0",
													"body":"Tx pin, used for flashing and debugging",
													"switches":[
														{
															"id":"pin1",
															"valueUnchecked":0,
															"labelUnchecked":"LOW",
															"valueChecked":1,
															"labelChecked":"HIGH",
															"defaultState":"unchecked",
															"action":"/digitalWrite?pin=1&value=%s"
														}
													]
												},
												{
													"icon": "pin_invoke",
													"label":"Digital Pin D2",
													"body":"Must be LOW during boot and also connected to the on-board LED",
													"switches":[
														{
															"id":"pin2",
															"valueUnchecked":0,
															"labelUnchecked":"LOW",
															"valueChecked":1,
															"labelChecked":"HIGH",
															"defaultState":"unchecked",
															"action":"/digitalWrite?pin=2&value=%s"
														}
													]
												},
												{
													"icon": "pin_invoke",
													"label":"Digital Pin RX0",
													"body":"Rx pin, used for flashing and debugging",
													"switches":[
														{
															"id":"pin3",
															"valueUnchecked":0,
															"labelUnchecked":"LOW",
															"valueChecked":1,
															"labelChecked":"HIGH",
															"defaultState":"unchecked",
															"action":"/digitalWrite?pin=3&value=%s"
														}
													]
												},
												{
													"icon": "pin_invoke",
													"label":"Digital Pin D4",
													"body":"",
													"switches":[
														{
															"id":"pin4",
															"valueUnchecked":0,
															"labelUnchecked":"LOW",
															"valueChecked":1,
															"labelChecked":"HIGH",
															"defaultState":"unchecked",
															"action":"/digitalWrite?pin=4&value=%s"
														}
													]
												},
												{
													"icon": "pin_invoke",
													"label":"Digital Pin D5",
													"body":"Must be HIGH during boot",
													"switches":[
														{
															"id":"pin5",
															"valueUnchecked":0,
															"labelUnchecked":"LOW",
															"valueChecked":1,
															"labelChecked":"HIGH",
															"defaultState":"unchecked",
															"action":"/digitalWrite?pin=5&value=%s"
														}
													]
												},
												{
													"icon": "pin_invoke",
													"label":"Digital Pin D6",
													"body":"Connected to Flash memory",
													"switches":[
														{
															"id":"pin6",
															"valueUnchecked":0,
															"labelUnchecked":"LOW",
															"valueChecked":1,
															"labelChecked":"HIGH",
															"defaultState":"unchecked",
															"action":"/digitalWrite?pin=6&value=%s"
														}
													]
												},
												{
													"icon": "pin_invoke",
													"label":"Digital Pin D7",
													"body":"Connected to Flash memory",
													"switches":[
														{
															"id":"pin7",
															"valueUnchecked":0,
															"labelUnchecked":"LOW",
															"valueChecked":1,
															"labelChecked":"HIGH",
															"defaultState":"unchecked",
															"action":"/digitalWrite?pin=7&value=%s"
														}
													]
												},
												{
													"icon": "pin_invoke",
													"label":"Digital Pin D8",
													"body":"Connected to Flash memory",
													"switches":[
														{
															"id":"pin8",
															"valueUnchecked":0,
															"labelUnchecked":"LOW",
															"valueChecked":1,
															"labelChecked":"HIGH",
															"defaultState":"unchecked",
															"action":"/digitalWrite?pin=8&value=%s"
														}
													]
												},
												{
													"icon": "pin_invoke",
													"label":"Digital Pin D9",
													"body":"Connected to Flash memory",
													"switches":[
														{
															"id":"pin9",
															"valueUnchecked":0,
															"labelUnchecked":"LOW",
															"valueChecked":1,
															"labelChecked":"HIGH",
															"defaultState":"unchecked",
															"action":"/digitalWrite?pin=9&value=%s"
														}
													]
												},
												{
													"icon": "pin_invoke",
													"label":"Digital Pin D10",
													"body":"Connected to Flash memory",
													"switches":[
														{
															"id":"pin10",
															"valueUnchecked":0,
															"labelUnchecked":"LOW",
															"valueChecked":1,
															"labelChecked":"HIGH",
															"defaultState":"unchecked",
															"action":"/digitalWrite?pin=10&value=%s"
														}
													]
												},
												{
													"icon": "pin_invoke",
													"label":"Digital Pin D11",
													"body":"Connected to Flash memory",
													"switches":[
														{
															"id":"pin11",
															"valueUnchecked":0,
															"labelUnchecked":"LOW",
															"valueChecked":1,
															"labelChecked":"HIGH",
															"defaultState":"unchecked",
															"action":"/digitalWrite?pin=11&value=%s"
														}
													]
												},
												{
													"icon": "pin_invoke",
													"label":"Digital Pin D12",
													"body":"must be LOW during boot",
													"switches":[
														{
															"id":"pin12",
															"valueUnchecked":0,
															"labelUnchecked":"LOW",
															"valueChecked":1,
															"labelChecked":"HIGH",
															"defaultState":"unchecked",
															"action":"/digitalWrite?pin=12&value=%s"
														}
													]
												},
												{
													"icon": "pin_invoke",
													"label":"Digital Pin D13",
													"body":"",
													"switches":[
														{
															"id":"pin13",
															"valueUnchecked":0,
															"labelUnchecked":"LOW",
															"valueChecked":1,
															"labelChecked":"HIGH",
															"defaultState":"unchecked",
															"action":"/digitalWrite?pin=13&value=%s"
														}
													]
												},
												{
													"icon": "pin_invoke",
													"label":"Digital Pin D14",
													"body":"",
													"switches":[
														{
															"id":"pin14",
															"valueUnchecked":0,
															"labelUnchecked":"LOW",
															"valueChecked":1,
															"labelChecked":"HIGH",
															"defaultState":"unchecked",
															"action":"/digitalWrite?pin=14&value=%s"
														}
													]
												},
												{
													"icon": "pin_invoke",
													"label":"Digital Pin D15",
													"body":"must be HIGH during boot, prevents startup log if pulled LOW",
													"switches":[
														{
															"id":"pin15",
															"valueUnchecked":0,
															"labelUnchecked":"LOW",
															"valueChecked":1,
															"labelChecked":"HIGH",
															"defaultState":"unchecked",
															"action":"/digitalWrite?pin=15&value=%s"
														}
													]
												},
												{
													"icon": "pin_invoke",
													"label":"Digital Pin RX2",
													"body":"",
													"switches":[
														{
															"id":"pin16",
															"label":"",
															"valueUnchecked":0,
															"labelUnchecked":"LOW",
															"valueChecked":1,
															"labelChecked":"HIGH",
															"defaultState":"unchecked",
															"action":"/digitalWrite?pin=16&value=%s"
														}
													]
												},
												{
													"icon": "pin_invoke",
													"label":"Digital Pin TX2",
													"body":"",
													"switches":[
														{
															"id":"pin17",
															"valueUnchecked":0,
															"labelUnchecked":"LOW",
															"valueChecked":1,
															"labelChecked":"HIGH",
															"defaultState":"unchecked",
															"action":"/digitalWrite?pin=17&value=%s"
														}
													]
												},
												{
													"icon": "pin_invoke",
													"label":"Digital Pin D18",
													"body":"",
													"switches":[
														{
															"id":"pin18",
															"valueUnchecked":0,
															"labelUnchecked":"LOW",
															"valueChecked":1,
															"labelChecked":"HIGH",
															"defaultState":"unchecked",
															"action":"/digitalWrite?pin=18&value=%s"
														}
													]
												},
												{
													"icon": "pin_invoke",
													"label":"Digital Pin D19",
													"body":"",
													"switches":[
														{
															"id":"pin19",
															"valueUnchecked":0,
															"labelUnchecked":"LOW",
															"valueChecked":1,
															"labelChecked":"HIGH",
															"defaultState":"unchecked",
															"action":"/digitalWrite?pin=19&value=%s"
														}
													]
												},
												{
													"icon": "pin_invoke",
													"label":"Digital Pin D21",
													"body":"",
													"switches":[
														{
															"id":"pin21",
															"valueUnchecked":0,
															"labelUnchecked":"LOW",
															"valueChecked":1,
															"labelChecked":"HIGH",
															"defaultState":"unchecked",
															"action":"/digitalWrite?pin=21&value=%s"
														}
													]
												},
												{
													"icon": "pin_invoke",
													"label":"Digital Pin D22",
													"body":"",
													"switches":[
														{
															"id":"pin22",
															"valueUnchecked":0,
															"labelUnchecked":"LOW",
															"valueChecked":1,
															"labelChecked":"HIGH",
															"defaultState":"unchecked",
															"action":"/digitalWrite?pin=22&value=%s"
														}
													]
												},
												{
													"icon": "pin_invoke",
													"label":"Digital Pin D23",
													"body":"",
													"switches":[
														{
															"id":"pin23",
															"valueUnchecked":0,
															"labelUnchecked":"LOW",
															"valueChecked":1,
															"labelChecked":"HIGH",
															"defaultState":"unchecked",
															"action":"/digitalWrite?pin=23&value=%s"
														}
													]
												},
												{
													"icon": "pin_invoke",
													"label":"Digital Pin D25",
													"body":"",
													"switches":[
														{
															"id":"pin25",
															"valueUnchecked":0,
															"labelUnchecked":"LOW",
															"valueChecked":1,
															"labelChecked":"HIGH",
															"defaultState":"unchecked",
															"action":"/digitalWrite?pin=25&value=%s"
														}
													]
												},
												{
													"icon": "pin_invoke",
													"label":"Digital Pin D26",
													"body":"",
													"switches":[
														{
															"id":"pin26",
															"valueUnchecked":0,
															"labelUnchecked":"LOW",
															"valueChecked":1,
															"labelChecked":"HIGH",
															"defaultState":"unchecked",
															"action":"/digitalWrite?pin=26&value=%s"
														}
													]
												},
												{
													"icon": "pin_invoke",
													"label":"Digital Pin D27",
													"body":"",
													"switches":[
														{
															"id":"pin27",
															"valueUnchecked":0,
															"labelUnchecked":"LOW",
															"valueChecked":1,
															"labelChecked":"HIGH",
															"defaultState":"unchecked",
															"action":"/digitalWrite?pin=27&value=%s"
														}
													]
												},
												{
													"icon": "pin_invoke",
													"label":"Digital Pin D32",
													"body":"",
													"switches":[
														{
															"id":"pin32",
															"valueUnchecked":0,
															"labelUnchecked":"LOW",
															"valueChecked":1,
															"labelChecked":"HIGH",
															"defaultState":"unchecked",
															"action":"/digitalWrite?pin=32&value=%s"
														}
													]
												},
												{
													"icon": "pin_invoke",
													"label":"Digital Pin D33",
													"body":"",
													"switches":[
														{
															"id":"pin33",
															"valueUnchecked":0,
															"labelUnchecked":"LOW",
															"valueChecked":1,
															"labelChecked":"HIGH",
															"defaultState":"unchecked",
															"action":"/digitalWrite?pin=33&value=%s"
														}
													]
												},
												{
													"icon": "pin_invoke",
													"label":"Digital Pin D34",
													"body":"Input only GPIO, cannot be configured as output",
													"switches":[
														{
															"id":"pin34",
															"valueUnchecked":0,
															"labelUnchecked":"LOW",
															"valueChecked":1,
															"labelChecked":"HIGH",
															"defaultState":"unchecked",
															"action":"/digitalWrite?pin=34&value=%s"
														}
													]
												},
												{
													"icon": "pin_invoke",
													"label":"Digital Pin D35",
													"body":"Input only GPIO, cannot be configured as output",
													"switches":[
														{
															"id":"pin35",
															"valueUnchecked":0,
															"labelUnchecked":"LOW",
															"valueChecked":1,
															"labelChecked":"HIGH",
															"defaultState":"unchecked",
															"action":"/digitalWrite?pin=35&value=%s"
														}
													]
												},
												{
													"icon": "pin_invoke",
													"label":"Digital Pin VP",
													"body":"Input only GPIO, cannot be configured as output",
													"switches":[
														{
															"id":"pin36",
															"valueUnchecked":0,
															"labelUnchecked":"LOW",
															"valueChecked":1,
															"labelChecked":"HIGH",
															"defaultState":"unchecked",
															"action":"/digitalWrite?pin=36&value=%s"
														}
													]
												},
												{
													"icon": "pin_invoke",
													"label":"Digital Pin VN",
													"body":"Input only GPIO, cannot be configured as output",
													"switches":[
														{
															"id":"pin39",
															"valueUnchecked":0,
															"labelUnchecked":"LOW",
															"valueChecked":1,
															"labelChecked":"HIGH",
															"defaultState":"unchecked",
															"action":"/digitalWrite?pin=39&value=%s"
														}
													]
												}
											]
										},
										"actions":{}
									},
									{
										"width":6,
										"title":"digital Read",
										"body":{
											"lists": [
												{
													"icon": "pin_invoke",
													"label":"pin D0",
													"body":"",
													"body_id":"trigger_pinD0",
													"buttons": [
														{
															"id":"pinD0",
															"label":"Read pin D0",
															"action":"/digitalRead?pin=0",
															"trigger":"trigger_pinD0",
															"class":"mdl-button--raised"
														}
													]
												},
												{
													"icon": "pin_invoke",
													"label":"pin D1",
													"body":"",
													"body_id":"trigger_pinD1",
													"buttons": [
														{
															"id":"pinD1",
															"label":"Read pin D1",
															"action":"/digitalRead?pin=1",
															"trigger":"trigger_pinD1",
															"class":"mdl-button--raised"
														}
													]
												},
												{
													"icon": "pin_invoke",
													"label":"pin D2",
													"body":"",
													"body_id":"trigger_pinD2",
													"buttons": [
														{
															"id":"pinD2",
															"label":"Read pin 2",
															"action":"/digitalRead?pin=2",
															"trigger":"trigger_pinD2",
															"class":"mdl-button--raised"
														}
													]
												},
												{
													"icon": "pin_invoke",
													"label":"pin RX0",
													"body":"",
													"body_id":"trigger_pinD3",
													"buttons": [
														{
															"id":"pinD3",
															"label":"Read pin 3",
															"action":"/digitalRead?pin=3",
															"trigger":"trigger_pinD3",
															"class":"mdl-button--raised"
														}
													]
												},
												{
													"icon": "pin_invoke",
													"label":"pin D4",
													"body":"",
													"body_id":"trigger_pinD4",
													"buttons": [
														{
															"id":"pinD4",
															"label":"Read pin 4",
															"action":"/digitalRead?pin=4",
															"trigger":"trigger_pinD4",
															"class":"mdl-button--raised"
														}
													]
												},
												{
													"icon": "pin_invoke",
													"label":"pin D5",
													"body":"",
													"body_id":"trigger_pinD5",
													"buttons": [
														{
															"id":"pinD5",
															"label":"Read pin 5",
															"action":"/digitalRead?pin=5",
															"trigger":"trigger_pinD5",
															"class":"mdl-button--raised"
														}
													]
												},
												{
													"icon": "pin_invoke",
													"label":"pin D6",
													"body":"",
													"body_id":"trigger_pinD6",
													"buttons": [
														{
															"id":"pinD6",
															"label":"Read pin 6",
															"action":"/digitalRead?pin=6",
															"trigger":"trigger_pinD6",
															"class":"mdl-button--raised"
														}
													]
												},
												{
													"icon": "pin_invoke",
													"label":"pin D7",
													"body":"",
													"body_id":"trigger_pinD7",
													"buttons": [
														{
															"id":"pinD7",
															"label":"Read pin 7",
															"action":"/digitalRead?pin=7",
															"trigger":"trigger_pinD7",
															"class":"mdl-button--raised"
														}
													]
												},
												{
													"icon": "pin_invoke",
													"label":"pin D8",
													"body":"",
													"body_id":"trigger_pinD8",
													"buttons": [
														{
															"id":"pinD8",
															"label":"Read pin 8",
															"action":"/digitalRead?pin=8",
															"trigger":"trigger_pinD8",
															"class":"mdl-button--raised"
														}
													]
												}
											]
										}
									}
								]
							}
						]
					}
				]
			}
		},
		{
			"name": "Analog",
			"id": "analog",
			"class": "",
			"icon": "timeline",
			"width":12,
			"body": {
				"rows":[
					{
						"row_id": "tab2_row1",
						"width":12,
						"columns":[
							{
								"width":12,
								"col_id":"C",
								"cards":[
									{
										"width":4,
										"title":"Analog",
										"body":{
											"slider":
												{
													"label":"A0",
													"id":"sliderA0",
													"type":"range",
													"width":12,
													"min":0,
													"max":100,
													"value":0,
													"step":1,
													"unit":"%s %",
													"action":"/analogWrite?pin=0&value=%s"
												}
										},
										"actions":{}
									},
									{
										"width":8,
										"title":"analogRead",
										"body":{
											"lists": [
												{
													"icon": "timeline",
													"label":"n/a",
													"label_id":"trigger_pinA0",
													"buttons": [
														{
															"id":"pinA0",
															"label":"analogRead 0",
															"action":"/analogRead?pin=0",
															"trigger":"trigger_pinA0",
															"class":"mdl-button--raised"
														}
													]
												}
											]
										}
									}
								]
							}
						]
					}
				]
			}
		},
		{
			"name": "Led RGB",
			"id": "ledrgb",
			"class": "",
			"icon": "wb_incandescent",
			"width":12,
			"body": {
				"rows":[
					{
						"row_id": "tab3_row1",
						"width":12,
						"columns":[
							{
								"width":12,
								"col_id":"tab3_row1_col1",
								"cards":[
									{
										"width":12,
										"title":"Led color",
										"body":{
											"texts":[
												{
													"text":"Choose Led color from sliders below:",
													"width":12,
													"id":"",
													"class":""
												}
											],
											"sliders":[
												{
													"label":"Red",
													"id":"sliderRed",
													"width":12,
													"min":0,
													"max":255,
													"value":0,
													"step":5,
													"unit":"",
													"action":"/analogWrite?pin=5&value=%s"
												},
												{
													"label":"Green",
													"id":"sliderGreen",
													"width":12,
													"min":0,
													"max":255,
													"value":0,
													"step":5,
													"unit":"",
													"action":"/analogWrite?pin=4&value=%s"
												},
												{
													"label":"Blue",
													"id":"sliderBlue",
													"width":12,
													"min":0,
													"max":255,
													"value":0,
													"step":5,
													"unit":"",
													"action":"/analogWrite?pin=16&value=%s"
												}
											]
										},
										"actions":{
											"buttons":[
												{
													"label":"get mode and values",
													"value":"",
													"action":"/getValues?pin=0,1,2,3,4,5,6,7,8,9",
													"trigger":"sensorValue"
												},
												{
													"label":"pinMode R",
													"value":"OUTPUT",
													"action":"/setPinModeOutput?pin=4",
													"trigger":"sensorValue"
												},
												{
													"label":"pinMode G",
													"value":"OUTPUT",
													"action":"/setPinModeOutput?pin=5",
													"trigger":"sensorValue"
												},
												{
													"label":"pinMode B",
													"value":"OUTPUT",
													"action":"/setPinModeOutput?pin=16",
													"trigger":"sensorValue"
												}
											]
										}
									}
								]
							}
						]
					}
				]
			}
		},
		{
			"name": "Arduino Audio",
			"id": "audio",
			"class": "",
			"icon": "volume_up",
			"width":12,
			"body": {
				"rows":[
					{
						"row_id": "tab4_row1",
						"width":12,
						"columns":[
							{
								"width":12,
								"col_id":"tab4_row1_col1",
								"cards":[
									{
										"width":12,
										"title":"Arduino Audio",
										"body":{
											"inputs":[
												{
													"label":"Say a message:",
													"placeholder":"Hello World",
													"pattern": "",
													"width":12,
													"id":"audioOutput",
													"class":""
												}
											],
											"buttons":[
												{
													"label":"Say",
													"value":"",
													"action":"/audioOutput?value=%s",
													"trigger":"audioOutput",
													"class":"mdl-button--raised"
												}
											]
										}
									}
								]
							}
						]
					}
				]
			}
		},
		{
			"name": "Help",
			"id": "help",
			"class": "",
			"icon": "contact_support",
			"width":12,
			"body": {
				"rows":[
					{
						"row_id": "tab5_row1",
						"width":12,
						"columns":[
							{
								"width":12,
								"col_id":"tab5_row1_col1",
								"cards":[
									{
										"width":12,
										"title":"How to use Arduino Universal Controller?",
										"body":{
											"texts":[
												{
													"text":"You first need to upload the sample generic Arduino source code to your device after editing the wifi credentials and t6 IoT Object settings.",
													"width":12,
													"id":"helptext",
													"class":""
												}
											]
										}
									}
								]
							}
						]
					}
				]
			}
		}
	],
	"footer": {
		"sections": [
			{
				"title": "Features",
				"links": [
					{
						"label": "t6 IoT",
						"href": "https://api.internetcollaboratif.info/"
					},
					{
						"label": "Features",
						"href": "https://www.internetcollaboratif.info/features"
					},
					{
						"label": "Articles",
						"href": "https://www.internetcollaboratif.info/post/"
					}
				]
			},
			{
				"title": "Technical details",
				"links": [
					{
						"label": "Github repository",
						"href": "https://github.com/mathcoll/t6"
					},
					{
						"label": "t6 Api doc",
						"href": "https://doc.internetcollaboratif.info/"
					},
					{
						"label": "Install IoT platform",
						"href": "https://github.com/mathcoll/t6/wiki/Installing-t6"
					},
					{
						"label": "t6 iot Arduino library",
						"href": "https://github.com/mathcoll/t6iot"
					}
				]
			}
		]
	}
};