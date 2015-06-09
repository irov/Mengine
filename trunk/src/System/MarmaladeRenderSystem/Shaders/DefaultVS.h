const char* defaultVS =
	"uniform highp mat4 mvpMat;			"
	"attribute highp vec4 inVert;		"
	"attribute mediump vec2 inUV0;		"
	"attribute mediump vec2 inUV1;		"
	"attribute lowp vec4 inCol;			"
	"									"
	"varying mediump vec2 v_UV0;		"
	"varying mediump vec2 v_UV1;		"
	"varying lowp vec4 v_Col;			"
	"void main(void)					"
	"{									"
	"	gl_Position = mvpMat * inVert;	"
	"									"
	"	v_UV0 = inUV0;					"
	"	v_UV1 = inUV1;					"
	"	v_Col = inCol;					"
	"}									";

