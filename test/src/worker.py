import shutil
import tempfile
import subprocess
import os

from generated.worker_pb2_grpc import WorkerStub
from generated.worker_pb2 import MapTask
from generated.common_pb2 import *


tools_dir = os.environ.get("TEST_HOME")
storage_dir = os.environ.get("STORAGE_BASE_DIR")
key_dir = os.environ.get("KEY_BASE_DIR")


def map_data_type_to_width(data_type: DataType):
    if data_type == UINT8 or data_type == INT8:
        return 8
    elif data_type == UINT16 or data_type == INT16:
        return 16
    elif data_type == UINT32 or data_type == INT32:
        return 32
    elif data_type == UINT64 or data_type == INT64:
        return 64

    assert False


def setup_resources(temp_dir,
                    session,
                    input_data_frame, output_data_frame, chunk_id,
                    key, row_size: int,
                    copy_frame, copy_key):
    subprocess.run([os.path.join(tools_dir, "data_generator"), str(row_size)], cwd=temp_dir)

    os.makedirs(os.path.join(storage_dir, session, input_data_frame), exist_ok=True)
    os.makedirs(os.path.join(storage_dir, session, output_data_frame), exist_ok=True)
    os.makedirs(os.path.join(key_dir, session), exist_ok=True)

    if copy_frame:
        shutil.copy(os.path.join(temp_dir, "data"), os.path.join(storage_dir, session, input_data_frame, str(chunk_id)))
    else:
        try:
            os.remove(os.path.join(storage_dir, session, input_data_frame, str(chunk_id)))
        except FileNotFoundError:
            pass

    if copy_key:
        shutil.copy(os.path.join(temp_dir, "key.pub"), os.path.join(key_dir, session, f"{key}.key"))
    else:
        try:
            os.remove(os.path.join(key_dir, session, f"{key}.key"))
        except FileNotFoundError:
            pass


def decrypt_data(temp_dir, session, output_data_frame, row_size, row_count):
    result = subprocess.run(
        [
            os.path.join(tools_dir, "dataframe_decryptor"),
            os.path.join(temp_dir, "key.pub"), os.path.join(temp_dir, "key"),
            os.path.join(storage_dir, session, output_data_frame), str(row_size), str(row_count)
        ], capture_output=True)
    return result.stdout.decode().split("\n")[:-1]


def single_frame_map_task(stub: WorkerStub, task: MapTask, copy_frame=True, copy_key=True):
    with tempfile.TemporaryDirectory() as tmp_dir_name:
        input_size = sum([map_data_type_to_width(column) for column in task.circuit.input])
        output_size = sum([map_data_type_to_width(column.data_type) for column in task.circuit.output])
        row_count = task.input_data_frame_ptr.row_count

        setup_resources(tmp_dir_name,
                        task.session_uuid,
                        task.input_data_frame_ptr.pointer.data_frame_uuid, task.output_data_frame_ptr.data_frame_uuid,
                        task.input_data_frame_ptr.pointer.partition,
                        1, #only BINFHE support
                        input_size,
                        copy_frame, copy_key)

        stub.map(task)

        output_data_frame_name = os.path.join(
            task.output_data_frame_ptr.data_frame_uuid,
            str(task.output_data_frame_ptr.partition))

        return decrypt_data(tmp_dir_name, task.session_uuid, output_data_frame_name, output_size, row_count)
