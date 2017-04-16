#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <map>
#include <cassert>

struct pair {
	size_t object_id;
	long double value;
	pair(size_t object_id_, long double value_)
		: object_id(object_id_),
		  value(value_) {}
};

typedef std::vector<pair> array;

array square(const array& arr) {
	array answer;
	answer.reserve(arr.size());
	for (auto& item: arr) {
		answer.push_back(pair(item.object_id, item.value * item.value));
	}
	return answer;
}

array multiply(const array& first, const array& second) {
	array answer;

	size_t i = 0;
	size_t j = 0;

	while (i < first.size() && j < second.size()) {
		while (i < first.size() && first[i].object_id < second[j].object_id) {
			++i;
		}
        if (i == first.size()) {
            break;
        }
		if (first[i].object_id == second[j].object_id) {
			answer.push_back(pair(first[i].object_id, first[i].value * second[j].value));
			++i;
			++j;
		} else {
			++j;
		}
	}

	return answer;
}

long double average(const array& arr, size_t size) {
	long double answer = 0;
	for (auto& item: arr) {
		answer += item.value;
	}
	return answer / size;
}

long double std_dev(const array& arr, size_t size) {
	long double avg = average(arr, size);
	return std::pow(average(square(arr), size) - avg * avg, 0.5);
}

long double correlation(const array& first, const array& second, size_t size) {
	long double a = average(multiply(first, second), size) - average(first, size) * average(second, size);

	return a / std_dev(first, size) / std_dev(second, size);
}

std::vector<array> read_X_train(size_t number_objects, size_t number_features,
								std::istream& input_stream = std::cin) {
	size_t number_nonzero;
	input_stream >> number_nonzero;

	std::vector<array> answer(number_features);

	for (size_t i = 0; i < number_nonzero; ++i) {
		size_t object_id, feature_id;
		long double value;
		input_stream >> object_id >> feature_id >> value;
		--object_id;
		--feature_id;
		answer[feature_id].emplace_back(object_id, value);
	}
	return answer;
}

std::vector<array> read_y_train(size_t number_objects, size_t number_labels,
								std::istream& input_stream = std::cin) {
 	std::vector<size_t> label_sizes;
 	for (size_t i = 0; i < number_objects; ++i) {
 		size_t current_number_labels;
 		input_stream >> current_number_labels;
 		label_sizes.push_back(current_number_labels);
 	}

 	std::vector<array> answer(number_labels);

 	for (size_t i = 0; i < number_objects; ++i) {
 		size_t id_object;
 		input_stream >> id_object;
 		--id_object;
 		assert(id_object == i);
 		for (size_t j = 0; j < label_sizes[i]; ++j) {
 			size_t current_label;
 			input_stream >> current_label;
 			--current_label;
 			answer[current_label].push_back(pair(id_object, 1));
 		}
 	}
 	return answer;
}

void write_correlations(std::vector<std::vector<long double>> correlations,
						std::ostream& output_stream = std::cout) {
	for (auto item: correlations) {
		for (auto second_item: item) {
			output_stream << second_item << " ";
		}
		output_stream << std::endl;
	}
}



int main() {
	size_t number_objects = 15000;
	size_t number_features = 30000;
 	size_t number_labels = 98;


	std::ifstream in_first("/home/yuru/Desktop/kaggle/contest_topic/X_trainC++.txt");

	auto X_train = read_X_train(number_objects, number_features, in_first);

	std::ifstream in_second("/home/yuru/Desktop/kaggle/contest_topic/y_trainC++.txt");

	auto y_train = read_y_train(number_objects, number_labels, in_second);

	std::vector<std::vector<long double>> correlations(X_train.size());
	for (size_t i = 0; i < X_train.size(); ++i) {
		for (auto& y: y_train) {
			correlations[i].push_back(correlation(X_train[i], y, number_objects));
		}
	}

    std::ofstream out("/home/yuru/Desktop/kaggle/contest_topic/correlations.txt");

	write_correlations(correlations, out);

	return 0;
}