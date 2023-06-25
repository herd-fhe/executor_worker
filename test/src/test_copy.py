import os

import pytest
import grpc

from generated.worker_pb2 import MapTask, DataFramePtr, CryptoKeyPtr, InputDataFramePtr
from generated.worker_pb2_grpc import WorkerStub
from generated.circuit_pb2 import Circuit
from generated.node_pb2 import InputNode, OutputNode, Node, OperationNode, NOT
from generated.common_pb2 import Edge, BINFHE

from worker import single_frame_map_task


@pytest.fixture()
def stub():
    port = os.environ.get("WORKER_PORT")
    channel = grpc.insecure_channel(f'localhost:{port}')
    stub = WorkerStub(channel)

    yield stub

    channel.close()


def test_copy_8(stub):
    task = MapTask(
        session_uuid="2ebb8249-0249-4d19-86f8-07ffa5c258cc",
        input_data_frame_ptr=InputDataFramePtr(
            pointer=DataFramePtr(
                data_frame_uuid="2ebb8249-0249-4d19-86f8-07ffa5c258cc",
                block_id=0
            ),
            row_count=16,
        ),
        output_data_frame_ptr=DataFramePtr(
            data_frame_uuid="f5a1afbc-7090-483b-8602-eaca0d5c7620",
            block_id=0
        ),
        crypto_key_ptr=CryptoKeyPtr(
            schema_type=BINFHE
        ),
        circuit=Circuit(
            input=[8],
            output=[8],
            nodes=[
                Node(input=InputNode(tuple_index=0, bit_index=0)),
                Node(input=InputNode(tuple_index=0, bit_index=1)),
                Node(input=InputNode(tuple_index=0, bit_index=2)),
                Node(input=InputNode(tuple_index=0, bit_index=3)),
                Node(input=InputNode(tuple_index=0, bit_index=4)),
                Node(input=InputNode(tuple_index=0, bit_index=5)),
                Node(input=InputNode(tuple_index=0, bit_index=6)),
                Node(input=InputNode(tuple_index=0, bit_index=7)),
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
               '11111111',
               '01111111',
               '00111111',
               '00011111',
               '00001111',
               '00000111',
               '00000011',
               '00000001',
               '00000000',
               '10000000',
               '11000000',
               '11100000',
               '11110000',
               '11111000',
               '11111100',
               '11111110'
           ] == result


def test_copy_16(stub):
    task = MapTask(
        session_uuid="2ebb8249-0249-4d19-86f8-07ffa5c258cc",
        input_data_frame_ptr=InputDataFramePtr(
            pointer=DataFramePtr(
                data_frame_uuid="2ebb8249-0249-4d19-86f8-07ffa5c258cc",
                block_id=1
            ),
            row_count=32,
        ),
        output_data_frame_ptr=DataFramePtr(
            data_frame_uuid="f5a1afbc-7090-483b-8602-eaca0d5c7620",
            block_id=1
        ),
        crypto_key_ptr=CryptoKeyPtr(
            schema_type=BINFHE
        ),
        circuit=Circuit(
            input=[16],
            output=[16],
            nodes=[
                Node(input=InputNode(tuple_index=0, bit_index=0)),
                Node(input=InputNode(tuple_index=0, bit_index=1)),
                Node(input=InputNode(tuple_index=0, bit_index=2)),
                Node(input=InputNode(tuple_index=0, bit_index=3)),
                Node(input=InputNode(tuple_index=0, bit_index=4)),
                Node(input=InputNode(tuple_index=0, bit_index=5)),
                Node(input=InputNode(tuple_index=0, bit_index=6)),
                Node(input=InputNode(tuple_index=0, bit_index=7)),
                Node(input=InputNode(tuple_index=0, bit_index=8)),
                Node(input=InputNode(tuple_index=0, bit_index=9)),
                Node(input=InputNode(tuple_index=0, bit_index=10)),
                Node(input=InputNode(tuple_index=0, bit_index=11)),
                Node(input=InputNode(tuple_index=0, bit_index=12)),
                Node(input=InputNode(tuple_index=0, bit_index=13)),
                Node(input=InputNode(tuple_index=0, bit_index=14)),
                Node(input=InputNode(tuple_index=0, bit_index=15)),
                Node(output=OutputNode(tuple_index=0, bit_index=0)),
                Node(output=OutputNode(tuple_index=0, bit_index=1)),
                Node(output=OutputNode(tuple_index=0, bit_index=2)),
                Node(output=OutputNode(tuple_index=0, bit_index=3)),
                Node(output=OutputNode(tuple_index=0, bit_index=4)),
                Node(output=OutputNode(tuple_index=0, bit_index=5)),
                Node(output=OutputNode(tuple_index=0, bit_index=6)),
                Node(output=OutputNode(tuple_index=0, bit_index=7)),
                Node(output=OutputNode(tuple_index=0, bit_index=8)),
                Node(output=OutputNode(tuple_index=0, bit_index=9)),
                Node(output=OutputNode(tuple_index=0, bit_index=10)),
                Node(output=OutputNode(tuple_index=0, bit_index=11)),
                Node(output=OutputNode(tuple_index=0, bit_index=12)),
                Node(output=OutputNode(tuple_index=0, bit_index=13)),
                Node(output=OutputNode(tuple_index=0, bit_index=14)),
                Node(output=OutputNode(tuple_index=0, bit_index=15))
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
                Edge(start=8, end=24),
                Edge(start=9, end=25),
                Edge(start=10, end=26),
                Edge(start=11, end=27),
                Edge(start=12, end=28),
                Edge(start=13, end=29),
                Edge(start=14, end=30),
                Edge(start=15, end=31),
            ]
        )
    )

    result = single_frame_map_task(stub, task)
    assert [
               '1111111111111111', '0111111111111111', '0011111111111111', '0001111111111111',
               '0000111111111111', '0000011111111111', '0000001111111111', '0000000111111111',
               '0000000011111111', '0000000001111111', '0000000000111111', '0000000000011111',
               '0000000000001111', '0000000000000111', '0000000000000011', '0000000000000001',
               '0000000000000000', '1000000000000000', '1100000000000000', '1110000000000000',
               '1111000000000000', '1111100000000000', '1111110000000000', '1111111000000000',
               '1111111100000000', '1111111110000000', '1111111111000000', '1111111111100000',
               '1111111111110000', '1111111111111000', '1111111111111100', '1111111111111110',
           ] == result


def test_copy_2_8(stub):
    task = MapTask(
        session_uuid="2ebb8249-0249-4d19-86f8-07ffa5c258cc",
        input_data_frame_ptr=InputDataFramePtr(
            pointer=DataFramePtr(
                data_frame_uuid="2ebb8249-0249-4d19-86f8-07ffa5c258cc",
                block_id=2
            ),
            row_count=32,
        ),
        output_data_frame_ptr=DataFramePtr(
            data_frame_uuid="f5a1afbc-7090-483b-8602-eaca0d5c7620",
            block_id=2
        ),
        crypto_key_ptr=CryptoKeyPtr(
            schema_type=BINFHE
        ),
        circuit=Circuit(
            input=[8, 8],
            output=[8, 8],
            nodes=[
                Node(input=InputNode(tuple_index=0, bit_index=0)),
                Node(input=InputNode(tuple_index=0, bit_index=1)),
                Node(input=InputNode(tuple_index=0, bit_index=2)),
                Node(input=InputNode(tuple_index=0, bit_index=3)),
                Node(input=InputNode(tuple_index=0, bit_index=4)),
                Node(input=InputNode(tuple_index=0, bit_index=5)),
                Node(input=InputNode(tuple_index=0, bit_index=6)),
                Node(input=InputNode(tuple_index=0, bit_index=7)),
                Node(output=OutputNode(tuple_index=0, bit_index=0)),
                Node(output=OutputNode(tuple_index=0, bit_index=1)),
                Node(output=OutputNode(tuple_index=0, bit_index=2)),
                Node(output=OutputNode(tuple_index=0, bit_index=3)),
                Node(output=OutputNode(tuple_index=0, bit_index=4)),
                Node(output=OutputNode(tuple_index=0, bit_index=5)),
                Node(output=OutputNode(tuple_index=0, bit_index=6)),
                Node(output=OutputNode(tuple_index=0, bit_index=7)),
                Node(input=InputNode(tuple_index=1, bit_index=0)),
                Node(input=InputNode(tuple_index=1, bit_index=1)),
                Node(input=InputNode(tuple_index=1, bit_index=2)),
                Node(input=InputNode(tuple_index=1, bit_index=3)),
                Node(input=InputNode(tuple_index=1, bit_index=4)),
                Node(input=InputNode(tuple_index=1, bit_index=5)),
                Node(input=InputNode(tuple_index=1, bit_index=6)),
                Node(input=InputNode(tuple_index=1, bit_index=7)),
                Node(output=OutputNode(tuple_index=1, bit_index=0)),
                Node(output=OutputNode(tuple_index=1, bit_index=1)),
                Node(output=OutputNode(tuple_index=1, bit_index=2)),
                Node(output=OutputNode(tuple_index=1, bit_index=3)),
                Node(output=OutputNode(tuple_index=1, bit_index=4)),
                Node(output=OutputNode(tuple_index=1, bit_index=5)),
                Node(output=OutputNode(tuple_index=1, bit_index=6)),
                Node(output=OutputNode(tuple_index=1, bit_index=7))
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

    result = single_frame_map_task(stub, task)
    assert [
               '1111111111111111', '0111111111111111', '0011111111111111', '0001111111111111',
               '0000111111111111', '0000011111111111', '0000001111111111', '0000000111111111',
               '0000000011111111', '0000000001111111', '0000000000111111', '0000000000011111',
               '0000000000001111', '0000000000000111', '0000000000000011', '0000000000000001',
               '0000000000000000', '1000000000000000', '1100000000000000', '1110000000000000',
               '1111000000000000', '1111100000000000', '1111110000000000', '1111111000000000',
               '1111111100000000', '1111111110000000', '1111111111000000', '1111111111100000',
               '1111111111110000', '1111111111111000', '1111111111111100', '1111111111111110',
           ] == result