import os

import pytest
import grpc

from generated.worker_pb2 import MapTask, DataFramePtr, CryptoKeyPtr, InputDataFramePtr
from generated.worker_pb2_grpc import WorkerStub
from generated.circuit_pb2 import Circuit
from generated.node_pb2 import InputNode, OutputNode, Node, OperationNode, NOT, ConstantNode
from generated.common_pb2 import Edge, BINFHE

from worker import single_frame_map_task


@pytest.fixture()
def stub():
    port = os.environ.get("WORKER_PORT")
    channel = grpc.insecure_channel(f'localhost:{port}')
    stub = WorkerStub(channel)

    yield stub

    channel.close()


def test_constant(stub):
    task = MapTask(
        session_uuid="2ebb8249-0249-4d19-86f8-07ffa5c258cc",
        input_data_frame_ptr=InputDataFramePtr(
            pointer=DataFramePtr(
                data_frame_uuid="2ebb8249-0249-4d19-86f8-07ffa5c258cc",
                partition=3
            ),
            row_count=4,
        ),
        output_data_frame_ptr=DataFramePtr(
            data_frame_uuid="f5a1afbc-7090-483b-8602-eaca0d5c7620",
            partition=3
        ),
        crypto_key_ptr=CryptoKeyPtr(
            schema_type=BINFHE
        ),
        circuit=Circuit(
            input=[8],
            output=[8],
            nodes=[
                Node(constant=ConstantNode(value=True)),
                Node(constant=ConstantNode(value=False)),
                Node(constant=ConstantNode(value=False)),
                Node(constant=ConstantNode(value=True)),
                Node(constant=ConstantNode(value=True)),
                Node(constant=ConstantNode(value=False)),
                Node(constant=ConstantNode(value=True)),
                Node(constant=ConstantNode(value=True)),
                Node(output=OutputNode(tuple_index=0, bit_index=0)),
                Node(output=OutputNode(tuple_index=0, bit_index=1)),
                Node(output=OutputNode(tuple_index=0, bit_index=2)),
                Node(output=OutputNode(tuple_index=0, bit_index=3)),
                Node(output=OutputNode(tuple_index=0, bit_index=4)),
                Node(output=OutputNode(tuple_index=0, bit_index=5)),
                Node(output=OutputNode(tuple_index=0, bit_index=6)),
                Node(output=OutputNode(tuple_index=0, bit_index=7))
            ],
            edges=[
                Edge(start=0, end=8),
                Edge(start=1, end=9),
                Edge(start=2, end=10),
                Edge(start=3, end=11),
                Edge(start=4, end=12),
                Edge(start=5, end=13),
                Edge(start=6, end=14),
                Edge(start=7, end=15),
            ]
        )
    )

    result = single_frame_map_task(stub, task)
    assert [
               '10011011', '10011011',
               '10011011', '10011011',
           ] == result