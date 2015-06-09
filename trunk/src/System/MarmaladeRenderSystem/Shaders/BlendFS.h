const char* blendFS =
	"uniform sampler2D inSampler0;						"
	"varying mediump vec2 v_UV0;						"
	"varying lowp vec4 v_Col;							"
	"													"
	"void main(void)									"
	"{													"
	"	mediump vec4 c0 = texture2D(inSampler0, v_UV0);	"
	"	mediump vec4 c = v_Col * c0;					"
	"	gl_FragColor = c;								"
	"}													";
