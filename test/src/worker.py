import logging
import shutil
import tempfile
import subprocess

from generated.worker_pb2_grpc import WorkerStub
import os

tools_dir = os.environ.get("TEST_HOME")
storage_dir = os.environ.get("STORAGE_BASE_DIR")
key_dir = os.environ.get("KEY_BASE_DIR")


def setup_resources(temp_dir, session, input_data_frame, output_data_frame, chunk_id, key, row_size: int):
    logging.info("Generate resources")
    subprocess.run([os.path.join(tools_dir, "data_generator"), str(row_size)], cwd=temp_dir)

    os.makedirs(os.path.join(storage_dir, session, input_data_frame), exist_ok=True)
    os.makedirs(os.path.join(storage_dir, session, output_data_frame), exist_ok=True)
    os.makedirs(os.path.join(key_dir, session), exist_ok=True)

    shutil.copy(os.path.join(temp_dir, "data"), os.path.join(storage_dir, session, input_data_frame, str(chunk_id)))
    shutil.copy(os.path.join(temp_dir, "key.pub"), os.path.join(key_dir, session, f"{key}.key"))


def decrypt_data(temp_dir, session, output_data_frame, row_size):
    result = subprocess.run(
        [
            os.path.join(tools_dir, "dataframe_decryptor"),
            os.path.join(temp_dir, "key.pub"), os.path.join(temp_dir, "key"),
            os.path.join(storage_dir, session, output_data_frame), str(row_size), str(row_size*2)
        ], capture_output=True)
    return result.stdout.decode().split("\n")[:-1]


def send_task(stub: WorkerStub, input_size: int, task):
    with tempfile.TemporaryDirectory() as tmp_dir_name:
        setup_resources(tmp_dir_name,
                        task.session_uuid,
                        task.input_data_frame_ptr.data_frame_uuid, task.output_data_frame_ptr.data_frame_uuid,
                        task.input_data_frame_ptr.block_id,
                        1, #only BINFHE support
                        input_size)

        result = stub.run(task)
        print(result)

        output_data_frame_name = os.path.join(
            task.output_data_frame_ptr.data_frame_uuid,
            str(task.output_data_frame_ptr.block_id))

        return decrypt_data(tmp_dir_name, task.session_uuid, output_data_frame_name, input_size)
