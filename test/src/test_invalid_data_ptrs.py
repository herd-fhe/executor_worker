import os
from typing import Tuple

import pytest
import grpc

from generated.worker_pb2 import MapTask, DataFramePtr, CryptoKeyPtr, InputDataFramePtr
from generated.worker_pb2_grpc import WorkerStub
from generated.circuit_pb2 import Circuit, OutputColumn, InputStructure
from generated.node_pb2 import OutputNode, Node, ConstantNode
from generated.common_pb2 import *

from worker import map_task, generate_data_frame


@pytest.fixture()
def stub():
    port = os.environ.get("WORKER_PORT")
    channel = grpc.insecure_channel(f'localhost:{port}')
    stub = WorkerStub(channel)

    yield stub

    channel.close()


def test_invalid_frame_ptr(stub, crypto_tool, session, key: Tuple[str, str]):
    task = MapTask(
        session_uuid=session,
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
            inputs=[
                InputStructure(
                    fields=[
                        INT8
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
                Node(constant=ConstantNode(value=True)),
                Node(constant=ConstantNode(value=False)),
                Node(constant=ConstantNode(value=False)),
                Node(constant=ConstantNode(value=True)),
                Node(constant=ConstantNode(value=True)),
                Node(constant=ConstantNode(value=False)),
                Node(constant=ConstantNode(value=True)),
                Node(constant=ConstantNode(value=True)),
                Node(output=OutputNode(field_index=0, bit_index=22)),
                Node(output=OutputNode(field_index=0, bit_index=1)),
                Node(output=OutputNode(field_index=0, bit_index=2)),
                Node(output=OutputNode(field_index=0, bit_index=3)),
                Node(output=OutputNode(field_index=0, bit_index=4)),
                Node(output=OutputNode(field_index=0, bit_index=5)),
                Node(output=OutputNode(field_index=0, bit_index=6)),
                Node(output=OutputNode(field_index=0, bit_index=7))
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

    with pytest.raises(grpc.RpcError) as e_info:
        map_task(stub, task)

    assert e_info.value.code() == grpc.StatusCode.FAILED_PRECONDITION


def test_invalid_key_ptr(stub, crypto_tool, key: Tuple[str, str]):
    session = "f5a1afbc-7090-483b-8602-eaca0d5c7620"
    context, private_key = key

    partition = 3
    input_data = [
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
    ]
    data_frame = generate_data_frame(crypto_tool, session, context, private_key, partition, input_data)

    task = MapTask(
        session_uuid=session,
        input_data_frame_ptr=InputDataFramePtr(
            pointer=DataFramePtr(
                data_frame_uuid=data_frame,
                partition=partition
            ),
            row_count=len(input_data),
        ),
        output_data_frame_ptr=DataFramePtr(
            data_frame_uuid="f5a1afbc-7090-483b-8602-eaca0d5c7620",
            partition=partition
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
                )
            ],
            output=[
                OutputColumn(
                    data_type=UINT8,
                    name="test"
                )
            ],
            nodes=[
                Node(constant=ConstantNode(value=True)),
                Node(constant=ConstantNode(value=False)),
                Node(constant=ConstantNode(value=False)),
                Node(constant=ConstantNode(value=True)),
                Node(constant=ConstantNode(value=True)),
                Node(constant=ConstantNode(value=False)),
                Node(constant=ConstantNode(value=True)),
                Node(constant=ConstantNode(value=True)),
                Node(output=OutputNode(field_index=0, bit_index=22)),
                Node(output=OutputNode(field_index=0, bit_index=1)),
                Node(output=OutputNode(field_index=0, bit_index=2)),
                Node(output=OutputNode(field_index=0, bit_index=3)),
                Node(output=OutputNode(field_index=0, bit_index=4)),
                Node(output=OutputNode(field_index=0, bit_index=5)),
                Node(output=OutputNode(field_index=0, bit_index=6)),
                Node(output=OutputNode(field_index=0, bit_index=7))
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

    with pytest.raises(grpc.RpcError) as e_info:
        map_task(stub, task)

    assert e_info.value.code() == grpc.StatusCode.FAILED_PRECONDITION
