<template>
  <div class="dashboard-container">
    <!-- 顶部用户信息（保持原有） -->
    <div class="user-info">
      <div class="greeting">
        <span class="hello">👋 欢迎回来，</span>
        <span class="name">{{ name }}</span>
    <div class="dashboard-text">用户角色:<span v-for="role in roles" :key="role">{{ role }}</span></div>
        <!-- <span class="role-tags">
          <el-tag
            v-for="role in (roles ? roles.split(',').map(r => r.trim()) : [])"
            :key="role"
            :type="role === 'admin' ? 'danger' : (role === 'editor' ? 'warning' : 'info')"
            size="small"
            effect="plain">
            {{ role === 'admin' ? '管理员' : (role === 'editor' ? '操作人' : (role === 'viewer' ? '访客' : '未知')) }}
          </el-tag>
        </span> -->
      </div>
      <!-- 可留空或放一个小装饰 -->
    </div>

    <!-- 中央时间卡片 -->
    <div class="time-card">
      <div class="solar-date">
        {{ solarYear }}年{{ solarMonth }}月{{ solarDay }}日 {{ weekDayChinese }}
      </div>
      <div class="solar-time">{{ hour }}:{{ minute }}:{{ second }}</div>
      <div class="lunar-info">
        <span class="lunar-date">农历 {{ lunarMonthChinese }}{{ lunarDayChinese }}</span>
        <span class="ganzhi">{{ ganzhiYear }}年 {{ ganzhiMonth }}月 {{ ganzhiDay }}日</span>
        <span class="hour-branch">时辰：{{ currentHourBranch }}</span>
        <span class="zodiac">生肖：{{ zodiac }}</span>
      </div>
    </div>

    <!-- 项目简介卡片 -->
    <div class="project-card">
      <h2 class="project-title">🎓 基于STM32、SpringBoot和Vue的中小型数据中心机房监控系统</h2>
      <div class="project-grid">
        <div class="project-section">
          <h3>📦 硬件清单</h3>
          <ul>
            <li>主控：STM32F103C8T6</li>
            <li>通信：ESP8266-01s (WiFi)</li>
            <li>传感器：DHT11 (温湿度) / MQ-2 (烟雾) / 光敏电阻 / 火焰传感器</li>
            <li>执行器：继电器（控制风扇）、LED灯、蜂鸣器</li>
            <li>显示：OLED屏幕</li>
          </ul>
        </div>
        <div class="project-section">
          <h3>⚙️ 软件技术</h3>
          <ul>
            <li>框架：Spring Boot 2.7 + MyBatisPlus</li>
            <li>数据库：MySQL (存储历史数据) + Redis (缓存用户权限菜单栏)</li>
            <li>权限系统：Spring Security + RABC</li>
            <li>协议：MQTT (与设备通信)</li>
            <li>前端：Vue2</li>
          </ul>
        </div>
        <div class="project-section">
          <h3>📱 功能特点</h3>
          <ul>
            <li>实时监控：温度、湿度、烟雾、光照、火焰</li>
            <li>远程控制：手动反控风扇、LED</li>
            <li>历史数据：曲线展示，支持按时间查询</li>
            <li>MQTT报文监控：查看原始通信数据</li>
            <li>报警推送：蜂鸣器本地报警</li>
          </ul>
        </div>
      </div>
    </div>

    <!-- 页脚装饰 -->
    <div class="footer-note">
      —— 基于STM32、SpringBoot和Vue的中小型数据中心机房监控系统 ——
    </div>
  </div>
</template>

<script>
import { mapGetters } from 'vuex'
import LunarCalendar from 'lunar-calendar' // 需要安装：npm install lunar-calendar

export default {
  name: 'Dashboard',
  data() {
    return {
      now: new Date(), // 当前时间，每秒更新
      lunar: {} // 农历信息对象
    }
  },
  computed: {
    ...mapGetters(['name', 'roles']),

    // 公历年月日
    solarYear() {
      return this.now.getFullYear()
    },
    solarMonth() {
      return String(this.now.getMonth() + 1).padStart(2, '0')
    },
    solarDay() {
      return String(this.now.getDate()).padStart(2, '0')
    },
    hour() {
      return String(this.now.getHours()).padStart(2, '0')
    },
    minute() {
      return String(this.now.getMinutes()).padStart(2, '0')
    },
    second() {
      return String(this.now.getSeconds()).padStart(2, '0')
    },
    weekDayChinese() {
      const weeks = ['日', '一', '二', '三', '四', '五', '六']
      return '星期' + weeks[this.now.getDay()]
    },

    // 农历月份（中文，考虑闰月）
    lunarMonthChinese() {
      if (!this.lunar.lunarMonth) return ''
      const monthStr = this.lunar.lunarMonth < 10 ? '十' : ''
      const chineseMonth = this.numberToChinese(this.lunar.lunarMonth)
      return (this.lunar.isLeap ? '闰' : '') + chineseMonth + '月'
    },
    // 农历日期（中文）
    lunarDayChinese() {
      if (!this.lunar.lunarDay) return ''
      return this.numberToChinese(this.lunar.lunarDay) + '日'
    },
    // 干支年
    ganzhiYear() {
      return this.lunar.GanZhiYear || ''
    },
    // 干支月
    ganzhiMonth() {
      return this.lunar.GanZhiMonth || ''
    },
    // 干支日
    ganzhiDay() {
      return this.lunar.GanZhiDay || ''
    },
    // 生肖
    zodiac() {
      return this.lunar.animal || this.lunar.zodiac || ''
    },
    // 当前时辰（地支）
    currentHourBranch() {
      const hour = this.now.getHours()
      // 地支对应表
      const branches = ['子', '丑', '寅', '卯', '辰', '巳', '午', '未', '申', '酉', '戌', '亥']
      // 时辰划分：23-01子，01-03丑，03-05寅，05-07卯，07-09辰，09-11巳，11-13午，13-15未，15-17申，17-19酉，19-21戌，21-23亥
      let index = Math.floor((hour + 1) / 2) % 12 // 公式：(hour+1)/2 取整，再模12，但需要处理23点
      // 23点属于子时（index 0），需要特殊处理
      if (hour === 23) index = 0
      return branches[index] + '时'
    }
  },
  mounted() {
    this.updateLunar()
    this.timer = setInterval(() => {
      this.now = new Date()
      this.updateLunar() // 农历每天变化，每秒更新也无妨
    }, 1000)
  },
  beforeDestroy() {
    clearInterval(this.timer)
  },
  methods: {
    // 更新农历信息
    updateLunar() {
      const y = this.now.getFullYear()
      const m = this.now.getMonth() + 1
      const d = this.now.getDate()
      this.lunar = LunarCalendar.solarToLunar(y, m, d)
    },
    // 数字转中文（1-30）
    numberToChinese(num) {
      const chineseNums = ['', '一', '二', '三', '四', '五', '六', '七', '八', '九', '十', '十一', '十二', '十三', '十四', '十五', '十六', '十七', '十八', '十九', '二十', '廿一', '廿二', '廿三', '廿四', '廿五', '廿六', '廿七', '廿八', '廿九', '三十']
      return chineseNums[num] || num
    }
  }
}
</script>

<style rel="stylesheet/scss" lang="scss" scoped>
.dashboard-container {
  margin: 30px;
  font-family: 'Helvetica Neue', Helvetica, 'PingFang SC', 'Hiragino Sans GB', 'Microsoft YaHei', Arial, sans-serif;
  background-color: #f5f7fa;
  min-height: calc(100vh - 60px);
  padding: 24px;
  border-radius: 16px;
}

.user-info {
  background: white;
  padding: 16px 24px;
  border-radius: 40px;
  box-shadow: 0 4px 12px rgba(0,0,0,0.03);
  margin-bottom: 30px;
  display: flex;
  justify-content: space-between;
  align-items: center;
  .greeting {
    display: flex;
    align-items: center;
    gap: 8px;
    font-size: 18px;
    .hello {
      color: #606266;
    }
    .name {
      font-weight: 600;
      color: #2c3e50;
    }
    .role-tags {
      margin-left: 8px;
    }
  }
}

.time-card {
  background: linear-gradient(145deg, #ffffff, #f9fafc);
  border-radius: 32px;
  padding: 40px 30px;
  box-shadow: 0 20px 40px rgba(0,0,0,0.05);
  text-align: center;
  margin-bottom: 40px;
  border: 1px solid rgba(255,255,255,0.5);
  .solar-date {
    font-size: 20px;
    color: #909399;
    letter-spacing: 1px;
    margin-bottom: 8px;
  }
  .solar-time {
    font-size: 64px;
    font-weight: 300;
    color: #2c3e50;
    line-height: 1.2;
    margin-bottom: 16px;
    font-family: 'Courier New', monospace;
  }
  .lunar-info {
    display: flex;
    flex-wrap: wrap;
    justify-content: center;
    gap: 16px 24px;
    font-size: 18px;
    color: #5a6a7a;
    background: #f0f2f6;
    padding: 16px 24px;
    border-radius: 60px;
    margin-top: 10px;
    span {
      white-space: nowrap;
    }
    .lunar-date {
      font-weight: 500;
      color: #b37f4b;
    }
    .ganzhi {
      color: #3b6e8f;
    }
    .hour-branch {
      color: #6b4f7c;
    }
    .zodiac {
      color: #c05a5a;
    }
  }
}

.project-card {
  background: white;
  border-radius: 28px;
  padding: 32px;
  box-shadow: 0 12px 30px rgba(0,0,0,0.05);
  .project-title {
    font-size: 24px;
    font-weight: 500;
    color: #2c3e50;
    text-align: center;
    margin: 0 0 30px 0;
    padding-bottom: 15px;
    border-bottom: 2px dashed #e0e4e8;
  }
  .project-grid {
    display: grid;
    grid-template-columns: repeat(3, 1fr);
    gap: 30px;
    @media (max-width: 800px) {
      grid-template-columns: 1fr;
    }
  }
  .project-section {
    h3 {
      font-size: 18px;
      font-weight: 600;
      color: #3c6e71;
      margin: 0 0 16px 0;
      display: flex;
      align-items: center;
      gap: 6px;
    }
    ul {
      list-style: none;
      padding: 0;
      margin: 0;
      li {
        padding: 8px 0;
        border-bottom: 1px solid #f0f2f5;
        color: #4a5568;
        &:last-child {
          border-bottom: none;
        }
        &::before {
          content: "•";
          color: #a0b8c9;
          font-weight: bold;
          display: inline-block;
          width: 20px;
        }
      }
    }
  }
}

.footer-note {
  text-align: center;
  margin-top: 40px;
  color: #b0bec5;
  font-size: 14px;
  font-style: italic;
}
</style>
