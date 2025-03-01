#pragma once

#include <cuda/runtime_api.hpp>

#include <gsl-lite/gsl-lite.hpp>

#include <cub/cub.cuh>

#include <thrustshift/not-a-vector.h>

namespace thrustshift {

namespace async {

template <class ValuesInRange,
          class ValuesOutRange,
          class ScanOp,
          class MemoryResource>
void inclusive_scan(cuda::stream_t& stream,
                    ValuesInRange&& values_in,
                    ValuesOutRange&& values_out,
                    ScanOp scan_op,
                    MemoryResource& delayed_memory_resource) {

	const std::size_t N = values_in.size();

	gsl_Expects(values_out.size() == N);

	size_t tmp_bytes_size = 0;
	void* tmp_ptr = nullptr;

	auto exec = [&] {
		cuda::throw_if_error(cub::DeviceScan::InclusiveScan(tmp_ptr,
		                                                    tmp_bytes_size,
		                                                    values_in.data(),
		                                                    values_out.data(),
		                                                    scan_op,
		                                                    N,
		                                                    stream.handle()));
	};
	exec();
	auto tmp =
	    make_not_a_vector<uint8_t>(tmp_bytes_size, delayed_memory_resource);
	tmp_ptr = tmp.to_span().data();
	exec();
}

} // namespace async

} // namespace thrustshift
