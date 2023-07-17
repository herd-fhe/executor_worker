import os

import pytest
import grpc

from generated.worker_pb2 import DataFramePtr, CryptoKeyPtr, InputDataFramePtr, ReduceTask
from generated.worker_pb2_grpc import WorkerStub
from generated.circuit_pb2 import Circuit, OutputColumn, InputStructure
from generated.node_pb2 import InputNode, OutputNode, Node, OperationNode, OR
from generated.common_pb2 import *

from worker import single_frame_reduce_task


@pytest.fixture()
def stub():
    port = os.environ.get("WORKER_PORT")
    channel = grpc.insecure_channel(f'localhost:{port}')
    stub = WorkerStub(channel)

    yield stub

    channel.close()


def test_reduce_or(stub):
    task = ReduceTask(
        session_uuid="2ebb8249-0249-4d19-86f8-07ffa5c258cc",
        input_data_frame_ptrs=[
            InputDataFramePtr(
                pointer=DataFramePtr(
                    data_frame_uuid="2ebb8249-0249-4d19-86f8-07ffa5c258cc",
                    partition=3
                ),
                row_count=16,
            )
        ],
        output_data_frame_ptr=DataFramePtr(
            data_frame_uuid="f5a1afbc-7090-483b-8602-eaca0d5cf620",
            partition=0
        ),
        crypto_key_ptr=CryptoKeyPtr(
            schema_type=BINFHE
        ),
        circuit=Circuit(
            inputs=[
                InputStructure(
                    fields=[
                        UINT8
                    ]
                ),
                InputStructure(
                    fields=[
                        UINT8
                    ]
                )
            ],
            output=[
                OutputColumn(
                    data_type=UINT8,
                    name="test"
                )
            ],
            nodes=[
                Node(input=InputNode(tuple_index=0, field_index=0, bit_index=0)),
                Node(input=InputNode(tuple_index=0, field_index=0, bit_index=1)),
                Node(input=InputNode(tuple_index=0, field_index=0, bit_index=2)),
                Node(input=InputNode(tuple_index=0, field_index=0, bit_index=3)),
                Node(input=InputNode(tuple_index=0, field_index=0, bit_index=4)),
                Node(input=InputNode(tuple_index=0, field_index=0, bit_index=5)),
                Node(input=InputNode(tuple_index=0, field_index=0, bit_index=6)),
                Node(input=InputNode(tuple_index=0, field_index=0, bit_index=7)),
                Node(input=InputNode(tuple_index=1, field_index=0, bit_index=0)),
                Node(input=InputNode(tuple_index=1, field_index=0, bit_index=1)),
                Node(input=InputNode(tuple_index=1, field_index=0, bit_index=2)),
                Node(input=InputNode(tuple_index=1, field_index=0, bit_index=3)),
                Node(input=InputNode(tuple_index=1, field_index=0, bit_index=4)),
                Node(input=InputNode(tuple_index=1, field_index=0, bit_index=5)),
                Node(input=InputNode(tuple_index=1, field_index=0, bit_index=6)),
                Node(input=InputNode(tuple_index=1, field_index=0, bit_index=7)),
                Node(operation=OperationNode(type=OR)),
                Node(operation=OperationNode(type=OR)),
                Node(operation=OperationNode(type=OR)),
                Node(operation=OperationNode(type=OR)),
                Node(operation=OperationNode(type=OR)),
                Node(operation=OperationNode(type=OR)),
                Node(operation=OperationNode(type=OR)),
                Node(operation=OperationNode(type=OR)),
                Node(output=OutputNode(field_index=0, bit_index=0)),
                Node(output=OutputNode(field_index=0, bit_index=1)),
                Node(output=OutputNode(field_index=0, bit_index=2)),
                Node(output=OutputNode(field_index=0, bit_index=3)),
                Node(output=OutputNode(field_index=0, bit_index=4)),
                Node(output=OutputNode(field_index=0, bit_index=5)),
                Node(output=OutputNode(field_index=0, bit_index=6)),
                Node(output=OutputNode(field_index=0, bit_index=7))
            ],
            edges=[
                Edge(start=0, end=16),
                Edge(start=1, end=17),
                Edge(start=2, end=18),
                Edge(start=3, end=19),
                Edge(start=4, end=20),
                Edge(start=5, end=21),
                Edge(start=6, end=22),
                Edge(start=7, end=23),
                Edge(start=8, end=16),
                Edge(start=9, end=17),
                Edge(start=10, end=18),
                Edge(start=11, end=19),
                Edge(start=12, end=20),
                Edge(start=13, end=21),
                Edge(start=14, end=22),
                Edge(start=15, end=23),
                Edge(start=16, end=24),
                Edge(start=17, end=25),
                Edge(start=18, end=26),
                Edge(start=19, end=27),
                Edge(start=20, end=28),
                Edge(start=21, end=29),
                Edge(start=22, end=30),
                Edge(start=23, end=31),
            ]
        )
    )

    result = single_frame_reduce_task(stub, task)
    assert [
               '11111111',
           ] == result