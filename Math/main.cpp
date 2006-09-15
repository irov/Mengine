#include	"vec2.h"
#include	"vec3.h"
#include	"vec4.h"
#include	"mat3.h"
#include	"mat4.h"
#include	"quat.h"
#include	"polygon.h"
#include	"stdio.h"

int	main()
{
	mt::vec2f	POINT(2.5f,1);

	mt::polygon	poly;
	poly.add_point(mt::vec2f(0.5,0.5));
	poly.add_point(mt::vec2f(3.0,0));
	poly.add_point(mt::vec2f(3,2.5));
	poly.add_point(mt::vec2f(2,2.6f));//--


	mt::vec2f	t(45,45);


	mt::polygon	poly1(poly);
	//poly1.add_point(mt::vec2f(1235,0.5));
//	poly1.add_point(mt::vec2f(3.0,0));
//	poly1.add_point(mt::vec2f(567,2.5));
//	poly1.add_point(mt::vec2f(2,2.6f));//--

	bool is = poly1 == poly;


	is = mt::is_point_inside_polygon(poly,POINT);

	is = mt::is_point_inside_convex_polygon(poly,POINT);
	is = mt::is_convex_polygon(poly);
	int y =mt::orient_polygon(poly);
	

	/*	testing	vec2f	*/
	mt::vec2f	b(0,0);

	b.x = 1.0f;
	b.y = 3.0f;

	mt::vec2f	c(b);

	b=c+b-c*3;

	float	val = mt::dot_v2_v2(c,b);
	float	len = c.length();

	mt::neg_v2(c);
	mt::norm_safe_v2(c,c);
	c=mt::norm_v2(c);
	len = c.length();

	/*	testing	vec3f	*/
	mt::vec3f	a(1,1,1);
	a[2] = 9;
	a+=mt::vec3f(0,5,5);
	a-=mt::vec3f(5,1,5);

	mt::vec3f	b1(a);

	mt::vec3f	c1(4,3,9);

	mt::neg_v3(a);

	a=b1+a-c1*2;

	a=mt::norm_safe_v3(a);
	len = a.length();

	mt::scale_v3_v3(a,a,3);

	len = a.length();

	/*	testing	vec4f	*/
	mt::vec4f	a1(1,1,1,5);
	a1[2] = 9;
	a1+=mt::vec4f(0,5,5,8);
	a1-=mt::vec4f(5,1,5,2);

	mt::vec4f	b2(a1);

	mt::vec4f	c2(4,3,9,45);

	mt::neg_v4(a1);

	a1=b2+a1-c2*2;

	a1=mt::norm_safe_v4(a1);
	len = a1.length();

	mt::scale_v4_v4(a1,a1,3);

	len = a1.length();

	mt::homogenize_v4(a1,a1);

	/*	mat3	*/
//	6,7,4
//	4,5,6
//	4,7,2

	mt::mat3f	A(mt::vec3f(6,7,4),mt::vec3f(4,5,6),mt::vec3f(4,7,2));
	mt::mat3f	B(mt::vec3f(6,7,4),mt::vec3f(4,5,6),mt::vec3f(4,7,2));

	mt::inv_m3(A,B);

	mt::ident_m3(A);

	mt::vec3f	vecB(1,2,3);

	mt::vec3f	res;

	mt::mul_m3_v3(res,B,vecB);

	mt::vec3f	res1;
	mt::mul_v3_m3(res1,vecB,B);

	mt::mat3f C = A * B;

	float det = mt::det_m3(A);

	/*	mat4	*/
	//	6,7,4,6
	//	4,5,6,8
	//	4,7,2,9
	//	4,6,3,1
	return 0;
};