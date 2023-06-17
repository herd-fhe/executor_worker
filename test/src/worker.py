import logging
import shutil
import tempfile
import subprocess

from generated.worker_pb2_grpc import WorkerStub
import os

tools_dir = os.environ.get("TEST_HOME")
storage_dir = os.environ.get("STORAGE_BASE_DIR")
key_dir = os.environ.get("KEY_BASE_DIR")


def setup_resources(temp_dir, session, input_data_frame, key, row_size: int):
    logging.info("Generate resources")
    subprocess.run([f"{tools_dir}/data_generator", str(row_size)], cwd=temp_dir)
    shutil.copy(f"{temp_dir}/data", f"{storage_dir}/{session}/{input_data_frame}")
    shutil.copy(f"{temp_dir}/key.pub", f"{key_dir}/{session}/{key}.key")


def decrypt_data(temp_dir, session, output_data_frame, row_size):
    result = subprocess.run(
        [
            f"{tools_dir}/table_decryptor",
            f"{temp_dir}/key.pub", f"{temp_dir}/key",
            f"{storage_dir}/{session}/{output_data_frame}", str(row_size), str(row_size*2)
        ], capture_output=True)
    return result.stdout.decode().split("\n")


def send_task(stub: WorkerStub, input_size: int, task):
    with tempfile.TemporaryDirectory() as tmp_dir_name:
        input_data_frame_name = f"{task.input_data_frame_ptr.data_frame_uuid}/{task.input_data_frame_ptr.block_id}"
        setup_resources(tmp_dir_name,
                        task.session_uuid, input_data_frame_name, task.crypto_key_ptr.schema_type,
                        input_size)

        result = stub.run(task)
        print(result)

        output_data_frame_name = f"{task.output_data_frame_ptr.data_frame_uuid}/{task.output_data_frame_ptr.block_id}"

        return decrypt_data(tmp_dir_name, task.session_uuid, output_data_frame_name, input_size)
