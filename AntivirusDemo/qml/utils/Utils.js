// SPDX-FileCopyrightText: 2024 Open Mobile Platform LLC <community@omp.ru>
// SPDX-License-Identifier: BSD-3-Clause

function convertLocalUrlToPath(url) {
    return url.toString();
}

function getAbsolutePath(path) {
    return convertLocalUrlToPath(path);
}

function getFileName(url) {
    return convertLocalUrlToPath(url).split("/").pop();
}
