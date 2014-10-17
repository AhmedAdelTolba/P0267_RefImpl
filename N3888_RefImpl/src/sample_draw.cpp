#include "sample_draw.h"
#include <cmath>
#include <random>
#include <vector>
#include <algorithm>
#include <string>
#include <sstream>

using namespace std;
using namespace std::experimental::io2d;

vector<vector<int>> init_sort_steps(int count, unsigned long mtSeed = 1009UL) {
	vector<vector<int>> result;
	result.push_back([count, mtSeed]() {
		vector<int> init;
		for (int i = 0; i < count; ++i) {
			init.push_back(i);
		}
		mt19937 rng(mtSeed);
		shuffle(begin(init), end(init), rng);
		return init;
	}());
	bool notSorted = true;
	while (notSorted) {
		vector<int> curr(result.back());
		const auto size = curr.size();
		notSorted = false;
		for (auto i = 0U; i < size - 1; ++i) {
			if (curr[i] > curr[i + 1]) {
				notSorted = true;
				auto temp = curr[i + 1];
				curr[i + 1] = curr[i];
				curr[i] = temp;
			}
		}
		if (notSorted) {
			result.push_back(curr);
		}
	}
	return result;
}

wostream& operator<<(wostream& os, const point& pt) {
	os << L"(" << pt.x << L"," << pt.y << L")";
	return os;
}

// Declaration
void draw_test_compositing_operators(surface& rs, double elapsedTimeInMilliseconds, compositing_operator secondRectCompOp, compositing_operator firstRectCompOp = compositing_operator::over, bool strokePaths = false, const rgba_color& backgroundColor = rgba_color::transparent_black, const rgba_color& firstColor = rgba_color::red * 0.8, const rgba_color& secondColor = rgba_color::teal * 0.4, bool clipToRects = false, bool clipToTriangle = false);

// Declaration
void draw_sort_visualization(surface& rs, double elapsedTimeInMilliseconds);

// Drawing entry point.
void sample_draw::operator()(surface& rs, double elapsedTimeInMilliseconds) {
	draw_sort_visualization(rs, elapsedTimeInMilliseconds);
	//draw_test_compositing_operators(rs, elapsedTimeInMilliseconds, compositing_operator::over);
}

// For testing purposes only.
void draw_test_compositing_operators(surface& rs, double /*elapsedTimeInMilliseconds*/, compositing_operator secondRectCompOp, compositing_operator firstRectCompOp, bool strokePaths, const rgba_color& backgroundColor, const rgba_color& firstColor, const rgba_color& secondColor, bool clipToRects, bool clipToTriangle) {
	// Parameter validation.
	if (clipToRects && clipToTriangle) {
		throw invalid_argument("clipToRects and clipToTriangle cannot both be set to true.");
	}
	rs.save();

	auto backgroundPattern = solid_color_pattern_builder(backgroundColor).get_pattern();
	auto firstPattern = solid_color_pattern_builder(firstColor).get_pattern();
	auto secondPattern = solid_color_pattern_builder(secondColor).get_pattern();

	auto pb = path_builder();

	pb.rect({ 10.0, 10.0, 120.0, 90.0 });
	auto firstRectPath = pb.get_path();

	pb.reset();
	pb.rect({ 50.0, 40.0, 120.0, 90.0 });
	auto secondRectPath = pb.get_path();

	pb.reset();
	pb.append_path(firstRectPath);
	pb.append_path(secondRectPath);
	auto bothRectsClipPath = pb.get_path();

	pb.reset();
	pb.move_to({ 85.0, 25.0 });
	pb.line_to({ 150.0, 115.0 });
	pb.line_to({ 30.0, 115.0 });
	pb.close_path();
	auto triangleClipPath = pb.get_path();

	rs.set_pattern(backgroundPattern);
	rs.set_compositing_operator(compositing_operator::source);
	rs.paint();

	rs.set_pattern(firstPattern);
	rs.set_compositing_operator(firstRectCompOp);
	rs.set_path(firstRectPath);
	rs.fill();


	if (clipToRects) {
		rs.set_path(bothRectsClipPath);
		rs.clip();
	}

	if (clipToTriangle) {
		rs.set_path(triangleClipPath);
		rs.clip();
	}

	rs.set_path(secondRectPath);
	rs.set_compositing_operator(secondRectCompOp);
	rs.set_pattern(secondPattern);
	rs.fill();

	rs.reset_clip();

	if (strokePaths) {
		rs.set_compositing_operator(compositing_operator::source);
		rs.set_line_width(2.0);

		rs.set_path(firstRectPath);
		rs.set_pattern(solid_color_pattern_builder(rgba_color::teal).get_pattern());
		rs.stroke();

		rs.set_path(secondRectPath);
		rs.set_pattern(solid_color_pattern_builder(rgba_color::red).get_pattern());
		rs.stroke();

		if (clipToTriangle) {
			rs.set_path(triangleClipPath);
			rs.set_pattern(solid_color_pattern_builder(rgba_color::yellow).get_pattern());
			rs.stroke();
		}
	}
	//rs.flush();
	//rs.write_to_png("D:\\michael\\00test2.png");

	rs.restore();
}

void draw_sort_visualization(surface& rs, double elapsedTimeInMilliseconds) {
	static double timer = 0.0;
	const double power = 3.0, lerpTime = 1250.0, phaseTime = lerpTime + 500.0, pi = 3.1415926535897932;
	const double normalizedTime = min(fmod(timer, phaseTime) / lerpTime, 1.0);
	const double adjustment = (normalizedTime < 0.5) ? pow(normalizedTime * 2.0, power) / 2.0 :
		((1.0 - pow(1.0 - ((normalizedTime - 0.5) * 2.0), power)) * 0.5) + 0.5;
	const int elementCount = 12;
	const static auto vec = init_sort_steps(elementCount);
	const int phaseCount = static_cast<int>(vec.size()), x = min(static_cast<int>(timer / phaseTime), phaseCount - 1);
	auto cornflowerBluePattern = solid_color_pattern_builder(rgba_color::cornflower_blue).get_pattern();
	rs.set_pattern(cornflowerBluePattern);
	rs.paint(); // Paint background.
	auto clextents = rs.get_clip_extents();
	const double radius = trunc(min(clextents.width * 0.8 / elementCount, clextents.height + 120.0) / 2.0);
	const double beginX = trunc(clextents.width * 0.1), y = trunc(clextents.height * 0.5);
	path_builder pb;
	pb.move_to({ beginX, 50.0 });
	rs.set_path(pb.get_path());
	auto whitePattern = solid_color_pattern_builder(rgba_color::white).get_pattern();
	rs.set_pattern(whitePattern);
	rs.select_font_face("Segoe UI", font_slant::normal, font_weight::normal);
	rs.set_font_size(40.0);
	rs.show_text(string("Phase ").append(to_string(x + 1)).c_str());

	for (int i = 0; i < elementCount; ++i) {
		pb.reset();
		const auto currVal = vec[x][i];
		if (x < phaseCount - 1) {
			const auto i2 = find(begin(vec[x + 1]), end(vec[x + 1]), currVal) - begin(vec[x + 1]);
			const auto x1r = radius * i * 2.0 + radius + beginX, x2r = radius * i2 * 2.0 + radius + beginX;
			const auto yr = y - ((i2 == i ? 0.0 : (radius * 4.0 * (normalizedTime < 0.5 ? normalizedTime : 1.0 - normalizedTime)))
				* (i % 2 == 1 ? 1.0 : -1.0));
			const auto center = point{ trunc((x2r - x1r) * adjustment + x1r), trunc(yr) };
			pb.set_transform_matrix(matrix_2d::init_scale({ 1.0, 1.5 }) * matrix_2d::init_rotate(pi / 4.0) * matrix_2d::init_translate({ 0.0, 50.0 }));
			pb.set_origin(center);
			pb.arc_negative(center, radius - 3.0, pi / 2.0, -pi / 2.0);
		}
		else {
			const point center{ radius * i * 2.0 + radius + beginX, y };
			pb.set_transform_matrix(matrix_2d::init_scale({ 1.0, 1.5 }) * matrix_2d::init_rotate(pi / 4.0));
			pb.set_origin(center);
			pb.arc_negative(center, radius - 3.0, pi / 2.0, -pi / 2.0);
		}
		rs.set_path(pb.get_path());
		double greyColor = 1.0 - (currVal / (elementCount - 1.0));
		auto greyPattern = solid_color_pattern_builder({ greyColor, greyColor, greyColor, 1.0 }).get_pattern();
		rs.set_pattern(greyPattern);
		rs.fill();
	}

	pb.reset();
	pb.set_origin({ 250.0, 450.0 });
	pb.set_transform_matrix(matrix_2d::init_shear_x(0.5).scale({ 2.0, 1.0 }));
	pb.rect({ 200.0, 400.0, 100.0, 100.0 });
	rs.set_path(pb.get_path());
	auto redPattern = solid_color_pattern_builder(rgba_color::red).get_pattern();
	rs.set_pattern(redPattern);
	rs.set_line_width(3.0);
	rs.stroke();
	auto radialPattern = radial_pattern_builder({ 250.0, 450.0 }, 0.0, { 250.0, 450.0 }, 80.0);
	radialPattern.add_color_stop_rgba(0.0, rgba_color::black);
	radialPattern.add_color_stop_rgba(0.25, rgba_color::red);
	radialPattern.add_color_stop_rgba(0.5, rgba_color::green);
	radialPattern.add_color_stop_rgba(0.75, rgba_color::blue);
	radialPattern.add_color_stop_rgba(1.0, rgba_color::white);
	radialPattern.set_extend(extend::reflect);
	rs.set_pattern(radialPattern.get_pattern());
	rs.fill();

	auto linearPattern = linear_pattern_builder({ 510.0, 460.0 }, { 530.0, 480.0 });
	linearPattern.add_color_stop_rgba(0.0, rgba_color::chartreuse);
	linearPattern.add_color_stop_rgba(1.0, rgba_color::salmon);
	linearPattern.set_extend(extend::repeat);
	pb.reset();
	pb.set_origin({ 500.0, 450.0 });
	pb.rect({ 500.0, 450.0, 100.0, 100.0 });
	pb.rect({ 525.0, 425.0, 50.0, 150.0 });
	pb.move_to({ 650.0, 400.0 });
	pb.rel_line_to({ 0.0, 100.0 });
	pb.rel_line_to({ 10.0, -100.0 });
	rs.set_line_join(line_join::miter);
	//rs.set_miter_limit(10000.0);
	rs.set_path(pb.get_path());
	rs.set_line_width(10.0);
	rs.set_pattern(redPattern);
	rs.stroke();
	rs.set_pattern(linearPattern.get_pattern());
	rs.fill();

	image_surface imgSurf(format::argb32, 10, 10);
	auto transparentYellow = solid_color_pattern_builder({ 0.5, 1.0, 0.8, 0.5 }).get_pattern();
	imgSurf.set_pattern(transparentYellow);
	imgSurf.paint();
	auto d1 = imgSurf.get_data();
	imgSurf.set_pattern(redPattern);
	pb.reset();
	pb.rect({ 0.0, 0.0, 2.0, 2.0 });
	imgSurf.set_path(pb.get_path());
	imgSurf.set_compositing_operator(compositing_operator::dest_atop);
	imgSurf.fill();
	auto d2 = imgSurf.get_data();
	//imgSurf.set_pattern()
	imgSurf.write_to_png("D:\\michael\\testpost4307.png");
	timer = (timer > phaseTime * (phaseCount + 2)) ? 0.0 : timer + elapsedTimeInMilliseconds;
}