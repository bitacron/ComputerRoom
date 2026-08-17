<script>
import { loadPermissions } from '@/utils/permission.js';

export default {
  onLaunch() {
    const token = uni.getStorageSync('access_token');
    if (!token) {
      uni.reLaunch({
        url: '/pages/login/login'
      });
      return;
    }
    loadPermissions().catch(() => {});
  }
};
</script>

<style>
page {
  background-color: #f5f7fa;
}
</style>
