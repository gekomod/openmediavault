<?php
/**
 * This file is part of OpenMediaVault.
 *
 * @license   https://www.gnu.org/licenses/gpl.html GPL Version 3
 * @author    Volker Theile <volker.theile@openmediavault.org>
 * @copyright Copyright (c) 2009-2025 Volker Theile
 *
 * OpenMediaVault is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * OpenMediaVault is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenMediaVault. If not, see <https://www.gnu.org/licenses/>.
 */
namespace Engined\Module;

class Rsync extends \OMV\Engine\Module\ServiceAbstract
		implements \OMV\Engine\Notify\IListener {
	public function getName() {
		return "rsync";
	}

	public function deployAfter(): array {
		return ["certificates"];
	}

	/**
	 * Helper function to find out whether the given shared folder
	 * configuration object is used. If it is used, then mark the
	 * module as dirty.
	 * @param type The event message type.
	 * @param path The event message path.
	 * @param object The configuration object.
	 */
	final public function onSharedFolder($type, $path, $object) {
		$db = \OMV\Config\Database::getInstance();
		if (TRUE === $db->exists("conf.service.rsync.job", [
			"operator" => "or",
			"arg0" => [
				"operator" => "stringEquals",
				"arg0" => "src",
				"arg1" => $object['uuid']
			],
			"arg1" => [
				"operator" => "stringEquals",
				"arg0" => "dest",
				"arg1" => $object['uuid']
			]
		])) {
			$this->setDirty();
		}
	}

	function bindListeners(\OMV\Engine\Notify\Dispatcher $dispatcher) {
		$dispatcher->addListener(
			OMV_NOTIFY_CREATE | OMV_NOTIFY_MODIFY | OMV_NOTIFY_DELETE,
			"org.openmediavault.conf.service.rsync.job",
			[$this, "setDirty"]);
		$dispatcher->addListener(
			OMV_NOTIFY_MODIFY,
			"org.openmediavault.conf.system.sharedfolder",
			[$this, "onSharedFolder"]);
	}
}
