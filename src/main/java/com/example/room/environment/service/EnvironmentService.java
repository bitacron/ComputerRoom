package com.example.room.environment.service;

import com.baomidou.mybatisplus.extension.plugins.pagination.Page;
import com.baomidou.mybatisplus.extension.service.IService;
import com.example.room.environment.entity.Environment;
import com.example.room.environment.entity.dto.EnvironmentQuery;
import com.example.room.environment.entity.dto.EnvironmentStatisticsQuery;

import javax.servlet.http.HttpServletResponse;
import java.io.IOException;
import java.io.UnsupportedEncodingException;
import java.util.List;

/**
 * <p>
 * 环境数据 服务类
 * </p>
 *
 * @author zhmy
 * @since 2023-05-31
 */
public interface EnvironmentService extends IService<Environment> {

    // 条件查询分页方法
    Page<Environment> pageQuery(EnvironmentQuery environmentQuery);

    /** 按条件全量列表（用于导出等，避免超大分页 LIMIT） */
    List<Environment> listByQuery(EnvironmentQuery query);

    Environment getLastData();

    Environment getLastData(String deviceKey);

    List<Environment> getStatistics(EnvironmentStatisticsQuery query);

    void exportExcel(List<Environment> list, HttpServletResponse response) throws IOException;

}
