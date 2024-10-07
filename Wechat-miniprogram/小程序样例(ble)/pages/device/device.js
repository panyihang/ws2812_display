const app = getApp();
var getScreenData = 0;

Page({
  data: {
    blockColor: "#ffffff",
    colorGamut: "-webkit-linear-gradient(left, #ffffff 0%, #ff0000 100%)",
    colorGray: "-webkit-linear-gradient(left, #000000 0%, #ffffff 100%)",
    colorGamutTip: "#ff0000",
    colorGrayTip: "#ffffff",
    colorValue: 100,
    colorGamutValue: 100,
    colorGrayValue: 100,
    selectedItem: 0, // 默认选中的显示项
    items: Array.from({
      length: 49
    }, (_, i) => i), // 生成1到50的选项数组
    playSpeed: 50,
    inputText: 'test',
    receiveText: '',
    name: '',
    connectedDeviceId: '',
    services: {
      0: 0x00ff
    },
    characteristics: {
      0: 0xff01,
      1: 0xff01
    },
    connected: true,
    list_height: 450,
    devicesList: [],
  },
  changeCorlor(e) {
    var value = e.detail.value
    var colors = []
    if (value >= 0 && value < 17) {
      colors = this.gradientColors("#ff0000", "#ffff00", 17, 2.2)
      value = value
    } else if (value >= 17 && value < 33) {
      colors = this.gradientColors("#ffff00", "#00ff00", 17, 2.2)
      value = value - 17
    } else if (value >= 33 && value < 50) {
      colors = this.gradientColors("#00ff00", "#00ffff", 17, 2.2)
      value = value - 33
    } else if (value >= 50 && value < 67) {
      colors = this.gradientColors("#00ffff", "#0000ff", 17, 2.2)
      value = value - 50
    } else if (value >= 67 && value < 83) {
      colors = this.gradientColors("#0000ff", "#ff00ff", 17, 2.2)
      value = value - 67
    } else {
      colors = this.gradientColors("#ff00ff", "#ff0000", 17, 2.2)
      value = value - 83
    }
    if (value >= colors.length) {
      value = value - 1
    }
    this.setData({
      colorValue: value,
      colorGamutTip: colors[value],
      colorGamut: "-webkit-linear-gradient(left, #ffffff 0%," + colors[value] + " 100%)"
    })

    var colorGamuts = []
    colorGamuts = this.gradientColors("#ffffff", this.data.colorGamutTip, 100, 2.2)
    this.setData({
      colorGrayTip: colorGamuts[this.data.colorGamutValue],
      colorGray: "-webkit-linear-gradient(left, #000000 0%," + colorGamuts[this.data.colorGamutValue] + " 100%)"
    })


    var colorGrays = []
    colorGrays = this.gradientColors("#000000", this.data.colorGrayTip, 100, 2.2)
    this.setData({
      blockColor: colorGrays[this.data.colorGrayValue]
    })
    console.log(this.data.blockColor);
    // 读取blockColor数据并转换为RGB格式
    const blockColor = this.data.blockColor.replace("#", "");
    const r = parseInt(blockColor.substr(0, 2), 16);
    const g = parseInt(blockColor.substr(2, 2), 16);
    const b = parseInt(blockColor.substr(4, 2), 16);

    // 调用封装的蓝牙发送函数
    const dataHead1 = 0x62;
    const dataHead2 = 0x00;
    const colorData = [r, g, b]; // RGB颜色数据

    this.sendBLEPackage(dataHead1, dataHead2, colorData);

  },
  changeCorlorGamut(e) {
    var value = e.detail.value
    var colorGamuts = []
    colorGamuts = this.gradientColors("#ffffff", this.data.colorGamutTip, 100, 2.2)
    if (value >= colorGamuts.length) {
      value = value - 1
    }
    this.setData({
      colorGamutValue: value,
      colorGrayTip: colorGamuts[value],
      colorGray: "-webkit-linear-gradient(left, #000000 0%," + colorGamuts[value] + " 100%)"
    })

    var colorGrays = []
    colorGrays = this.gradientColors("#000000", this.data.colorGrayTip, 100, 2.2)
    this.setData({
      blockColor: colorGrays[this.data.colorGrayValue]
    })
    console.log(this.data.blockColor);
    // 读取blockColor数据并转换为RGB格式
    const blockColor = this.data.blockColor.replace("#", "");
    const r = parseInt(blockColor.substr(0, 2), 16);
    const g = parseInt(blockColor.substr(2, 2), 16);
    const b = parseInt(blockColor.substr(4, 2), 16);

    // 调用封装的蓝牙发送函数
    const dataHead1 = 0x62;
    const dataHead2 = 0x00;
    const colorData = [r, g, b]; // RGB颜色数据

    this.sendBLEPackage(dataHead1, dataHead2, colorData);
  },
  changeCorlorGray(e) {
    var value = e.detail.value
    var colorGrays = []
    colorGrays = this.gradientColors("#000000", this.data.colorGrayTip, 100, 2.2)
    if (value >= colorGrays.length) {
      value = value - 1
    }
    this.setData({
      colorGrayValue: value,
      blockColor: colorGrays[value],
    })
    console.log(this.data.blockColor);
    // 读取blockColor数据并转换为RGB格式
    const blockColor = this.data.blockColor.replace("#", "");
    const r = parseInt(blockColor.substr(0, 2), 16);
    const g = parseInt(blockColor.substr(2, 2), 16);
    const b = parseInt(blockColor.substr(4, 2), 16);

    // 调用封装的蓝牙发送函数
    const dataHead1 = 0x62;
    const dataHead2 = 0x00;
    const colorData = [r, g, b]; // RGB颜色数据

    this.sendBLEPackage(dataHead1, dataHead2, colorData);
  },

  parseColor: function (hexStr) {
    return hexStr.length === 4 ? hexStr.substr(1).split('').map(function (s) {
      return 0x11 * parseInt(s, 16);
    }) : [hexStr.substr(1, 2), hexStr.substr(3, 2), hexStr.substr(5, 2)].map(function (s) {
      return parseInt(s, 16);
    })
  },

  // zero-pad 1 digit to 2
  pad: function (s) {
    return (s.length === 1) ? '0' + s : s;
  },

  gradientColors: function (start, end, steps, gamma) {
    var i, j, ms, me, output = [],
      so = [];
    gamma = gamma || 1;
    var normalize = function (channel) {
      return Math.pow(channel / 255, gamma);
    };
    start = this.parseColor(start).map(normalize);
    end = this.parseColor(end).map(normalize);
    for (i = 0; i < steps; i++) {
      ms = i / (steps - 1);
      me = 1 - ms;
      for (j = 0; j < 3; j++) {
        so[j] = this.pad(Math.round(Math.pow(start[j] * me + end[j] * ms, 1 / gamma) * 255).toString(16));
      }
      output.push('#' + so.join(''));
    }
    return output;
  },
  // 滑块值变化事件处理函数
  onSliderChange: function (e) {
    this.setData({
      playSpeed: e.detail.value
    });
    //speed
    var dataHead1 = 0x60;
    var dataHead2 = 0x00;
    var speed = this.data.playSpeed;
    var dataBody = [speed]; // 假设数据体是一个包含速度值的数组
    this.sendBLEPackage(dataHead1, dataHead2, dataBody);
  },
  // 下拉选择框值变化事件处理函数
  onPickerChange: function (e) {
    this.setData({
      selectedItem: e.detail.value
    });
    //chat_id
    var dataHead11 = 0x61;
    var dataHead21 = 0x00;
    var displayItem1 = [e.detail.value]; // 假设数据体是一个包含显示项值的数组
    this.sendBLEPackage(dataHead11, dataHead21, displayItem1);
    this.getData();

  },
  // 输入框绑定
  bindInput: function (e) {
    this.setData({
      inputText: e.detail.value
    });
    console.log(e.detail.value);
  },
  // 封装的蓝牙发送函数
  sendBLEPackage: function (dataHead1, dataHead2, dataBody, withBody = true) {
    const that = this;
    // 检查蓝牙连接状态等...

    // 创建数据头缓冲区
    const header = new Uint8Array([dataHead1, dataHead2]);

    // 如果指定了发送数据体，则创建数据体缓冲区
    let finalBuffer;
    if (withBody) {
      // 创建数据体缓冲区
      const body = new Uint8Array(dataBody);

      // 创建最终的ArrayBuffer，并将数据头和数据体合并
      finalBuffer = new Uint8Array(header.length + body.length);
      finalBuffer.set(header, 0);
      finalBuffer.set(body, header.length);
    } else {
      // 只发送数据头
      finalBuffer = header;
    }

    // 发送数据
    wx.writeBLECharacteristicValue({
      deviceId: that.data.connectedDeviceId,
      serviceId: that.data.services[0].uuid,
      characteristicId: that.data.characteristics[0].uuid,
      value: finalBuffer.buffer,
      success: function (res) {
        console.log('发送指令成功:' + res.errMsg);
        that.showToast('成功', 'success', 500);
      },
      fail: function (res) {
        console.log('message发送失败:' + res.errMsg);
        that.showToast('失败', 'warn', 2000);
      }
    });
  },
  // 发送获取全部数据的请求
  getAll: function () {
    console.log('触发全部');
    const that = this;

    if (!that.data.connected) {
      that.showBluetoothDisconnectedModal();
      return;
    }

    getScreenData = 101;
    let successCount = 0;

    for (let i = 2; i <= 51; i++) {
      let buffer = that.createBuffer(i);

      wx.writeBLECharacteristicValue({
        deviceId: that.data.connectedDeviceId,
        serviceId: that.data.services[0].uuid,
        characteristicId: that.data.characteristics[0].uuid,
        value: buffer,
        success: function (res) {
          console.log('发送指令成功:' + res.errMsg);
          getScreenData = 102 + successCount;
          successCount++;
          that.showToast('正在获取第' + successCount + '条数据', 'none', 100);

          if (successCount === 50) {
            that.showToast('全部发送成功', 'success', 2000);
          }
        },
        fail: function (res) {
          console.log('message发送失败:' + res.errMsg);
          that.showToast('发送失败', 'warn', 2000);
        }
      });
    }
  },

  // 发送获取单条数据的请求
  getData: function () {
    console.log('触发');
    const that = this;

    if (!that.data.connected) {
      that.showBluetoothDisconnectedModal();
      return;
    }

    getScreenData = 0;
    let buffer = that.createBuffer(0x01);

    wx.writeBLECharacteristicValue({
      deviceId: that.data.connectedDeviceId,
      serviceId: that.data.services[0].uuid,
      characteristicId: that.data.characteristics[0].uuid,
      value: buffer,
      success: function (res) {
        console.log('发送指令成功:' + res.errMsg);
        that.showToast('成功', 'success', 500);
      },
      fail: function (res) {
        console.log('message发送失败:' + res.errMsg);
        that.showToast('发送失败', 'warn', 2000);
      }
    });
  },

  // 发送文本数据
  Send: function () {
    const that = this;

    if (!that.data.connected) {
      that.showBluetoothDisconnectedModal();
      return;
    }

    getScreenData = 0;

    // 将输入文本转换为UTF-8编码数组
    const utf8Array = that.toUTF8Array(that.data.inputText);

    // 创建包含0x00 0x00的头部缓冲区
    const header = new Uint8Array([0x00, 0x00]);

    // 创建最终的ArrayBuffer，并将header和utf8Array合并
    const finalBuffer = new Uint8Array(header.length + utf8Array.length);
    finalBuffer.set(header, 0);
    finalBuffer.set(utf8Array, header.length);

    console.log('发送数据....');
    console.log(that.data.inputText);
    console.log(utf8Array);
    console.log(finalBuffer);

    wx.writeBLECharacteristicValue({
      deviceId: that.data.connectedDeviceId,
      serviceId: that.data.services[0].uuid,
      characteristicId: that.data.characteristics[0].uuid,
      value: finalBuffer.buffer,
      success: function (res) {
        console.log('发送指令成功:' + res.errMsg);
        that.showToast('成功', 'success', 500);
      },
      fail: function (res) {
        console.log('message发送失败:' + res.errMsg);
        that.showToast('失败', 'warn', 2000);
      }
    });
  },
  // 重启按钮点击事件处理函数
  reset: function () {
    const that = this;
    // 检查蓝牙连接状态等...

    // 调用封装的蓝牙发送函数
    const dataHead1 = 0xff;
    const dataHead2 = 0x00;
    const resetData = [0x01]; // 重启数据

    that.sendBLEPackage(dataHead1, dataHead2, resetData);
  },
  // 页面加载时的初始化
  onLoad: function (options) {
    const that = this;
    console.log(options);

    that.setData({
      name: options.name,
      connectedDeviceId: options.connectedDeviceId,
    });

    wx.getBLEDeviceServices({
      deviceId: that.data.connectedDeviceId,
      success: function (res) {
        console.log(res.services);
        that.setData({
          services: res.services
        });
        that.initializeBLECharacteristics(options.connectedDeviceId, res.services[0].uuid);
      }
    });

    wx.onBLEConnectionStateChange(function (res) {
      console.log(res.connected);
      that.setData({
        connected: res.connected
      });
    });
  },

  // 初始化 BLE 特征值
  initializeBLECharacteristics: function (deviceId, serviceId) {
    const that = this;

    wx.getBLEDeviceCharacteristics({
      deviceId: deviceId,
      serviceId: serviceId,
      success: function (res) {
        console.log(res.characteristics);
        that.setData({
          characteristics: res.characteristics
        });

        wx.notifyBLECharacteristicValueChange({
          state: true,
          deviceId: deviceId,
          serviceId: serviceId,
          characteristicId: that.data.characteristics[0].uuid,
          success: function (res) {
            console.log('启用notify成功：' + that.data.characteristics[0].uuid);
            console.log(JSON.stringify(res));
            wx.setBLEMTU({
              deviceId: deviceId,
              mtu: 500
            });
            that.onBLECharacteristicValueChange();
          },
          fail: function () {
            console.log('开启notify失败' + that.characteristicId);
          }
        });
      }
    });
  },

  // 监听 BLE 特征值变化
  onBLECharacteristicValueChange: function () {
    const that = this;

    wx.onBLECharacteristicValueChange(function (res) {
      const receiveText = that.arrayBufferToString(res.value);
      console.log('监听低功耗蓝牙设备的特征值变化事件成功');
      console.log(getScreenData);
      console.log(receiveText);

      that.processReceivedData(receiveText);
    });
  },

  // 处理接收到的数据
  processReceivedData: function (receiveText) {
    const that = this;

    switch (getScreenData) {
      case 0:
        that.setData({
          inputText: receiveText
        });
        break;
      case 1:
        that.setData({
          inputText: receiveText
        });
        break;
    }

    if (getScreenData > 100 && getScreenData <= 150) {
      const deviceId = getScreenData - 100;
      const existingDeviceIndex = that.data.devicesList.findIndex(device => device.deviceId === deviceId.toString());

      if (existingDeviceIndex !== -1) {
        that.data.devicesList[existingDeviceIndex].data = receiveText;
      } else {
        that.data.devicesList.push({
          deviceId: deviceId.toString(),
          data: receiveText
        });

      }
      that.setData({
        devicesList: that.data.devicesList
      });
    }
  },

  // 工具函数：创建ArrayBuffer
  createArrayBuffer: function (dataArray) {
    const buffer = new ArrayBuffer(dataArray.length);
    const dataView = new Uint8Array(buffer);
    for (let i = 0; i < dataArray.length; i++) {
      dataView[i] = dataArray[i];
    }
    return buffer;
  },

  // 工具函数：创建数据缓冲区
  createBuffer: function (value) {
    const buffer = new ArrayBuffer(2);
    const dataView = new DataView(buffer);
    dataView.setUint8(1, 0x00);
    dataView.setUint8(0, value);
    return buffer;
  },

  // 工具函数：将字符串转换为UTF-8编码数组
  toUTF8Array: function (str) {
    const utf8 = [];
    for (let i = 0; i < str.length; i++) {
      const charcode = str.charCodeAt(i);
      if (charcode < 0x80) utf8.push(charcode);
      else if (charcode < 0x800) {
        utf8.push(0xc0 | (charcode >> 6), 0x80 | (charcode & 0x3f));
      } else if (charcode < 0xd800 || charcode >= 0xe000) {
        utf8.push(0xe0 | (charcode >> 12), 0x80 | ((charcode >> 6) & 0x3f), 0x80 | (charcode & 0x3f));
      } else {
        i++;
        charcode = 0x10000 + (((charcode & 0x3ff) << 10) | (str.charCodeAt(i) & 0x3ff));
        utf8.push(0xf0 | (charcode >> 18), 0x80 | ((charcode >> 12) & 0x3f), 0x80 | ((charcode >> 6) & 0x3f), 0x80 | (charcode & 0x3f));
      }
    }
    return utf8;
  },

  // 工具函数：将ArrayBuffer转换为字符串
  arrayBufferToString: function (buffer) {
    const arrayBuffer = new Uint8Array(buffer);
    let str = '';
    for (let i = 0; i < arrayBuffer.length; i++) {
      str += '%' + ('00' + arrayBuffer[i].toString(16)).slice(-2);
    }
    return decodeURIComponent(str);
  },

  // 工具函数：显示提示信息
  showToast: function (title, icon, duration) {
    wx.showToast({
      title,
      icon,
      mask: true,
      duration
    });
  },

  // 工具函数：显示蓝牙断开提示
  showBluetoothDisconnectedModal: function () {
    wx.showModal({
      title: '提示',
      content: '蓝牙已断开',
      showCancel: false,
      success: (res) => {
        this.setData({
          searching: false
        });
      }
    });
  },
});