import os

import pytest
import grpc

from generated.worker_pb2 import MapTask, DataFramePtr, CryptoKeyPtr, InputDataFramePtr
from generated.worker_pb2_grpc import WorkerStub
from generated.circuit_pb2 import Circuit
from generated.node_pb2 import InputNode, OutputNode, Node, OperationNode, NOT, ConstantNode, OR, AND
from generated.common_pb2 import Edge, BINFHE

from worker import single_frame_map_task


@pytest.fixture()
def stub():
    port = os.environ.get("WORKER_PORT")
    channel = grpc.insecure_channel(f'localhost:{port}')
    stub = WorkerStub(channel)

    yield stub

    channel.close()


def test_and(stub):
    task = MapTask(
        session_uuid="2ebb8249-0249-4d19-86f8-07ffa5c258cc",
        input_data_frame_ptr=InputDataFramePtr(
            pointer=DataFramePtr(
                data_frame_uuid="2ebb8249-1249-4d19-86f8-07ffa5c258cc",
                block_id=3
            ),
            row_count=16,
        ),
        output_data_frame_ptr=DataFramePtr(
            data_frame_uuid="f5a1afbc-7190-483b-8602-eaca0d5cf620",
            block_id=3
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
                Node(input=InputNode(tuple_index=0, bit_index=0)),
                Node(input=InputNode(tuple_index=0, bit_index=1)),
                Node(input=InputNode(tuple_index=0, bit_index=2)),
                Node(input=InputNode(tuple_index=0, bit_index=3)),
                Node(input=InputNode(tuple_index=0, bit_index=4)),
                Node(input=InputNode(tuple_index=0, bit_index=5)),
                Node(input=InputNode(tuple_index=0, bit_index=6)),
                Node(input=InputNode(tuple_index=0, bit_index=7)),
                Node(operation=OperationNode(type=AND)),
                Node(operation=OperationNode(type=AND)),
                Node(operation=OperationNode(type=AND)),
                Node(operation=OperationNode(type=AND)),
                Node(operation=OperationNode(type=AND)),
                Node(operation=OperationNode(type=AND)),
                Node(operation=OperationNode(type=AND)),
                Node(operation=OperationNode(type=AND)),
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
                Edge(start=2, end=10),
                Edge(start=3, end=11),
                Edge(start=4, end=12),
                Edge(start=5, end=13),
                Edge(start=6, end=14),
                Edge(start=7, end=15),
                Edge(start=8, end=16),
                Edge(start=9, end=17),
                Edge(start=0, end=10),
                Edge(start=0, end=11),
                Edge(start=0, end=12),
                Edge(start=0, end=13),
                Edge(start=1, end=14),
                Edge(start=1, end=15),
                Edge(start=1, end=16),
                Edge(start=1, end=17),
                Edge(start=10, end=18),
                Edge(start=11, end=19),
                Edge(start=12, end=20),
                Edge(start=13, end=21),
                Edge(start=14, end=22),
                Edge(start=15, end=23),
                Edge(start=16, end=24),
                Edge(start=17, end=25),
            ]
        )
    )

    result = single_frame_map_task(stub, task)
    assert [
               '11110000',
               '01110000',
               '00110000',
               '00010000',
               '00000000',
               '00000000',
               '00000000',
               '00000000',
               '00000000',
               '10000000',
               '11000000',
               '11100000',
               '11110000',
               '11110000',
               '11110000',
               '11110000'
           ] == result