#!/bin/sh

curl -LO https://github.com/cmosher01/Apple-II-System-Masters/releases/download/v1.1.0/apple2sysmas.zip
unzip apple2sysmas.zip
curl -LO https://github.com/cmosher01/Apple-II-Source/releases/download/v1.1.2/apple2sys.zip
unzip apple2sys.zip

mkdir stage

cp ./apple2/dos/controller/13sector/disk2.a65              ./stage/13-disk2.a65
cp ./apple2/dos/controller/16sector/disk2.a65              ./stage/16-disk2.a65
cp ./apple2/system/monitor/apple2/monitor.a65              ./stage/a2-monitor.a65
cp ./apple2/system/intbasic/intbasic.a65                   ./stage/a2-intbasic.a65
cp ./apple2/system/other/other.a65                         ./stage/a2-other.a65
cp ./apple2/system/monitor/apple2plus/monitor.a65          ./stage/a2p-monitor.a65
cp ./apple2/system/applesoft/applesoft.a65                 ./stage/a2p-applesoft.a65

cp ./apple2/dos/system/dos310/original31sysmas.d13.woz     ./stage/original31sysmas.d13.woz
cp ./apple2/dos/system/dos320/original32sysmaspls.d13.woz  ./stage/original32sysmaspls.d13.woz
cp ./apple2/dos/system/dos320/original32sysmasstd.d13.woz  ./stage/original32sysmasstd.d13.woz
cp ./apple2/dos/system/dos321/original321sysmaspls.d13.woz ./stage/original321sysmaspls.d13.woz
cp ./apple2/dos/system/dos321/original321sysmasstd.d13.woz ./stage/original321sysmasstd.d13.woz
cp ./apple2/dos/system/dos330/original330sysmas.do.woz     ./stage/original330sysmas.do.woz
cp ./apple2/dos/system/dos331/original331sysmas.do.woz     ./stage/original331sysmas.do.woz
cp ./apple2/dos/system/dos332/original332sysmas.do.woz     ./stage/original332sysmas.do.woz
