"""Testcase for performing hard reset in the ECU Reset Service Identifier (0x11)."""
from time import sleep
from random import randint
from expliot.core.tests.test import Test, TCategory, TTarget, \
    TLog, LOGNORMAL
from expliot.core.protocols.hardware.can import CanBus, CanMessage


class ECUHardReset(Test):

    def __init__(self):
        """Initialize the test."""
        super().__init__(
            name="ecuhardreset",
            summary="ECU Hard Reset",
            descr="This plugin allows you to perform hard reset in the ECU Reset Service Identifier (0x11)"
            author="Jay Turla",
            email="shipcodez@pgmail.com",
            ref=[""],
            category=TCategory(TCategory.CAN, TCategory.HW, TCategory.ECUHARDRESET),
            target=TTarget(TTarget.GENERIC, TTarget.GENERIC, TTarget.GENERIC),
        )
        self.argparser.add_argument(
            "-i", "--iface", default="vcan0", help="Interface to use. Default is vcan0"
        )
        self.argparser.add_argument(
            "-a",
            "--arbitid",
            required=True,
            default="0x7DF"
            type=lambda x: int(x, 0),
            help="Specify the arbitration ID. For hex value prefix it with 0x",
        )
        self.argparser.add_argument(
            "-d",
            "--data",
            required=True,
            default="0211010000000000"
            help="Specify the frame data to use, as hex stream without the 0x prefix ",)
        self.argparser.add_argument(
            "-c",
            "--count",
            type=int,
            default=1,
            help="Specify the no. of messages to write. Default is 1",
        )
        self.argparser.add_argument(
            "-w",
            "--wait",
            type=float,
            default=0,
            help="Specify the wait time, in seconds, between each consecutive "
            "message write. Default is to not wait between writes. You "
            "may use float values as well, e.g. 0.5",
        )

    def execute(self):
        """Execute the test."""
        TLog.generic(
            "Performing ECU Hard Reset to CANbus on interface({}), arbitration id(0x{:x}), "
            "extended?({}) data({})".format(
                self.args.iface, self.args.arbitid, self.args.exid, self.args.data
            )
        )
        bus = None
        try:
            if self.args.count < 1:
                raise ValueError(
                    "Illegal count value {}".format(
                        self.args.count))
            if self.args.wait < 0:
                raise ValueError(
                    "Illegal wait value {}".format(
                        self.args.wait))
            bus = CanBus(bustype="socketcan", channel=self.args.iface)
            for count in range(1, self.args.count + 1):
                datacan = self.args.data
                while datacan.find("xx") >= 0:
                    datacan = datacan.replace("xx", "{:02x}".format(
                        randint(0, 0xFF)), 1)  # main fuzzing magic with randint
                message = CanMessage(
                    arbitration_id=self.args.arbitid,
                    data=list(
                        bytes.fromhex(datacan)))
                bus.send(message)
                self.output_handler(logkwargs=LOGNORMAL, count=count, fuzzdata=datacan)
                if self.args.wait > 0:
                    sleep(self.args.wait)
        except BaseException:
            self.result.exception()
        finally:
            if bus:
                bus.shutdown()
