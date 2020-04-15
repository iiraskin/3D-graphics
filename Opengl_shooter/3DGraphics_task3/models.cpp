#include "models.hpp"

void make_tree(Tree* res, const std::vector<GLfloat>& tree, const std::vector<GLfloat>& tree_colors, 
	           double turn_xy, double turn_xz, double scale, double bias_x, double bias_y, double bias_z)
{
	// ����� ������
	std::vector<GLfloat> tree_copy = tree;

	// ������� � ��������� X-Y
	for (int i = 0; i < tree_copy.size(); i += 3) {
		float a = tree_copy[i];
		float b = tree_copy[i + 1];
		tree_copy[i] = a * cos(turn_xy) - b * sin(turn_xy);
		tree_copy[i + 1] = b * cos(turn_xy) + a * sin(turn_xy);
	}

	// ������� � ��������� X-Z
	for (int i = 0; i < tree_copy.size(); i += 3) {
		float a = tree_copy[i];
		float b = tree_copy[i + 2];
		tree_copy[i] = a * cos(turn_xz) - b * sin(turn_xz);
		tree_copy[i + 2] = b * cos(turn_xz) + a * sin(turn_xz);
	}

	// �������� � ���������������.
	for (int i = 0; i < tree_copy.size(); i += 3) {
		tree_copy[i] = tree_copy[i] / scale + bias_x;
		tree_copy[i + 1] = tree_copy[i + 1] / scale + bias_y;
		tree_copy[i + 2] = tree_copy[i + 2] / scale + bias_z;
	}

	res->tree = tree_copy;
	res->tree_colors = tree_colors;
}


void MakeSpruce::add_tree(Spruce* res)
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(0, 10000);

	res->turn_xy = 2 * PI * dis(gen) / 10000.0;
	res->turn_xz = 2 * PI * dis(gen) / 10000.0;
	res->scale = dis(gen) / 2000.0 + 5;
	res->bias_x = dis(gen) / 500.0 - 10;
	res->bias_y = dis(gen) / 500.0 - 10;
	res->bias_z = dis(gen) / 500.0 - 10;

	make_tree(res, tree, tree_colors, res->turn_xy, res->turn_xz, res->scale, res->bias_x, res->bias_y, res->bias_z);

	res->radius = 0.3 / res->scale;
	res->centre = { res->tree[108], res->tree[109], res->tree[110] };
}

void MakeStump::add_tree(Stump* res, const Spruce& spruce)
{
	// ��� ��������� ��� � ���������������� ������.
	make_tree(res, stump, stump_colors, spruce.turn_xy, spruce.turn_xz, spruce.scale, spruce.bias_x, spruce.bias_y, spruce.bias_z);
	res->birth = glfwGetTime();
}

MakeFireboll::MakeFireboll()
{
	int N_PSI_part = N_PSI * 2 / 3;
	boll.resize(2 * N_PSI_part * N_PHI * 9);
	boll_colors.resize(2 * N_PSI_part * N_PHI * 9);

	glm::vec3 vertices[N_PSI][N_PHI];

	for (int j = 1; j < N_PSI_part; ++j) {
		for (int i = 0; i < N_PHI; ++i) {
			float phi = i * 2 * PI / N_PHI;
			float psi = j * PI / N_PSI;
			vertices[j][i] = glm::vec3(sin(psi) * cos(phi), sin(psi) * sin(phi), cos(psi));
		}
	}

	glm::vec3 north_pole = vec3(0., 0., 1.);
	glm::vec3 south_pole = vec3(0., 0., -2);

	int k = 0;

	//add triangles for south pole
	for (int i = 0; i < N_PHI; ++i) {
		boll[k++] = north_pole.x;
		boll[k++] = north_pole.y;
		boll[k++] = north_pole.z;
		boll_colors[k - 2] = north_pole.z;

		boll[k++] = vertices[1][i].x;
		boll[k++] = vertices[1][i].y;
		boll[k++] = vertices[1][i].z;
		boll_colors[k - 2] = vertices[1][i].z;

		boll[k++] = vertices[1][(i + 1) % N_PHI].x;
		boll[k++] = vertices[1][(i + 1) % N_PHI].y;
		boll[k++] = vertices[1][(i + 1) % N_PHI].z;
		boll_colors[k - 2] = vertices[1][(i + 1) % N_PHI].z;
	}

	//add triangles for north pole
	for (int i = 0; i < N_PHI; ++i) {
		boll[k++] = south_pole.x;
		boll[k++] = south_pole.y;
		boll[k++] = south_pole.z;
		boll_colors[k - 3] = 0.2;

		boll[k++] = vertices[N_PSI_part - 1][i].x;
		boll[k++] = vertices[N_PSI_part - 1][i].y;
		boll[k++] = vertices[N_PSI_part - 1][i].z;
		boll_colors[k - 2] = vertices[N_PSI_part - 1][i].z;


		boll[k++] = vertices[N_PSI_part - 1][(i + 1) % N_PHI].x;
		boll[k++] = vertices[N_PSI_part - 1][(i + 1) % N_PHI].y;
		boll[k++] = vertices[N_PSI_part - 1][(i + 1) % N_PHI].z;
		boll_colors[k - 2] = vertices[N_PSI_part - 1][(i + 1) % N_PHI].z;
	}

	//add all sphere triangles
	for (int j = 1; j < N_PSI_part - 1; ++j) {
		for (int i = 0; i < N_PHI; ++i) {
			boll[k++] = vertices[j][i].x;
			boll[k++] = vertices[j][i].y;
			boll[k++] = vertices[j][i].z;
			boll_colors[k - 2] = vertices[j][i].z;

			boll[k++] = vertices[j + 1][i].x;
			boll[k++] = vertices[j + 1][i].y;
			boll[k++] = vertices[j + 1][i].z;
			boll_colors[k - 2] = vertices[j + 1][i].z;

			boll[k++] = vertices[j + 1][(i + 1) % N_PHI].x;
			boll[k++] = vertices[j + 1][(i + 1) % N_PHI].y;
			boll[k++] = vertices[j + 1][(i + 1) % N_PHI].z;
			boll_colors[k - 2] = vertices[j + 1][(i + 1) % N_PHI].z;

			//////

			boll[k++] = vertices[j][i].x;
			boll[k++] = vertices[j][i].y;
			boll[k++] = vertices[j][i].z;
			boll_colors[k - 2] = vertices[j][i].z;

			boll[k++] = vertices[j + 1][(i + 1) % N_PHI].x;
			boll[k++] = vertices[j + 1][(i + 1) % N_PHI].y;
			boll[k++] = vertices[j + 1][(i + 1) % N_PHI].z;
			boll_colors[k - 2] = vertices[j + 1][(i + 1) % N_PHI].z;

			boll[k++] = vertices[j][(i + 1) % N_PHI].x;
			boll[k++] = vertices[j][(i + 1) % N_PHI].y;
			boll[k++] = vertices[j][(i + 1) % N_PHI].z;
			boll_colors[k - 2] = vertices[j][(i + 1) % N_PHI].z;
		}
	}

	for (int i = 0; i < k; ++i) {
		if (boll_colors[i] < 0)
			boll_colors[i] = -boll_colors[i];
	}

	// ��������� �������� � ������ ������. ������ ��� ���������� �� ����������� �������� �� �������.
	for (int i = 0; i < k; i += 3) {
		boll_colors[i] = 1. - boll_colors[i]; // ������ �� ����� ��� 0.
		//boll_colors[i + 2] = 0.;
	}

	for (int i = 0; i < k; ++i) {
		boll[i] /= scale;
	}
}

void MakeFireboll::add_boll(glm::vec3 position, glm::vec3 direction, Fireboll* res)
{
	std::vector<GLfloat> boll_copy = boll;
	float a = direction.x;
	float b = direction.y;
	float c = direction.z;
	float d = sqrt(a * a + c * c);

	glm::vec3 z = direction;
	glm::vec3 x = {c / d, 0., -a / d};
	d = sqrt(b * b * x .z * x.z + (c * x.x - a * x.z) * (c * x.x - a * x.z) + b * b * x.x * x.x);
	glm::vec3 y = {b * x.z / d, (c * x.x - a * x.z) / d, -b * x.x / d};
	//�������
	for (int i = 0; i < boll_copy.size(); i += 3) {
		float p = boll_copy[i];
		float q = boll_copy[i + 1];
		float r = boll_copy[i + 2];
		boll_copy[i] = p * x.x + q * y.x + r * z.x;
		boll_copy[i + 1] = p * x.y + q * y.y + r * z.y;
		boll_copy[i + 2] = p * x.z + q * y.z + r * z.z;
	}

	//��������
	for (int i = 0; i < boll_copy.size(); i += 3) {
		boll_copy[i] += position.x;
		boll_copy[i + 1] += position.y;
		boll_copy[i + 2] += position.z;
	}

	res->boll = boll_copy;
	res->boll_colors = boll_colors;
	res->direction = direction;
	res->radius = 1.0 / scale;
	res->centre = position;
	res->birth = glfwGetTime();
}

void MakeFireboll::find_position(Fireboll* res)
{
	double delta_time = glfwGetTime() - res->birth;
	for (int i = 0; i < res->boll.size(); i += 3) {
		res->boll[i] += res->direction.x * delta_time * res->speed;
		res->boll[i + 1] += res->direction.y * delta_time * res->speed;
		res->boll[i + 2] += res->direction.z * delta_time * res->speed;
	}
	res->centre += res->direction * (float)delta_time * res->speed;
}
