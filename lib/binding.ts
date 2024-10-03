var addon = require("bindings")("stellarsolver-native");
import { EventEmitter } from "events";

export enum ScaleUnits {
  DEG_WIDTH,
  ARCMIN_WIDTH,
  ARCSEC_PER_PIX,
  FOCAL_MM,
}

export enum ParametersProfile {
  DEFAULT,
  SINGLE_THREAD_SOLVING,
  PARALLEL_LARGESCALE,
  PARALLEL_SMALLSCALE,
  ALL_STARS,
  SMALL_STARS,
  MID_STARS,
  BIG_STARS,
}

export enum ProcessType {
  EXTRACT,
  EXTRACT_WITH_HFR,
  SOLVE,
}

export enum ExtractorType {
  INTERNAL,
  EXTERNAL,
  BUILTIN,
}

export enum SolverType {
  STELLARSOLVER,
  LOCALASTROMETRY,
  ASTAP,
  WATNEYASTROMETRY,
  ONLINEASTROMETRY,
}

export interface Star {
  x: number;
  y: number;
  hfr: number;
  flux: number;
}

export interface Solution {
  ra: number;
  dec: number;
  raError: number;
  decError: number;
  fieldHeight: number;
  fieldWidth: number;
  orientation: number;
  parity: number;
  pixscale: number;
}

interface FitsImageStatistic {
  new (): FitsImageStatistic;
  min: [number, number, number];
  max: [number, number, number];
  median: [number, number, number];
  dataType: number;
  samplesPerChannel: number;
  width: number;
  height: number;
  channels: number;
}

interface StellarSolverNative {
  start(): void;
  stop(): void;
  process(): void;
  getStarList(): Star[];
  getStarListFromSolve(): Star[];
  getSolution(): Solution;
  setProperty(prop: string, value: string | number | boolean): void;
  setParameterProfile(value: number): void;
  setSSLogLevel(value: number): void;
  setIndexFolderPaths(value: string): void;
  setPosition(ra: number, dec: number): void;
  setScale(low: number, high: number, unit: ScaleUnits): void;
  extractionDone(): boolean;
  solvingDone(): boolean;
  finished(): boolean;
}

export class StellarSolver extends EventEmitter {
  private _addonInstance: StellarSolverNative;

  constructor(stat: FitsImageStatistic, buffer: Buffer) {
    super();
    this._addonInstance = new addon.SS(stat, buffer, this.emit.bind(this));
  }

  start() {
    this._addonInstance.start();
  }

  stop() {
    this._addonInstance.stop();
  }

  process() {
    this._addonInstance.process();
  }

  getStarList() {
    return this._addonInstance.getStarList();
  }

  getStarListFromSolve() {
    return this._addonInstance.getStarListFromSolve();
  }

  getSolution() {
    return this._addonInstance.getSolution();
  }

  setProperty(prop: string, value: string | number | boolean) {
    this._addonInstance.setProperty(prop, value);
  }

  setParameterProfile(value: number) {
    this._addonInstance.setParameterProfile(value);
  }

  setSSLogLevel(value: number) {
    this._addonInstance.setParameterProfile(value);
  }

  setIndexFolderPaths(value: string) {
    this._addonInstance.setIndexFolderPaths(value);
  }

  setPosition(ra: number, dec: number) {
    this._addonInstance.setPosition(ra, dec);
  }

  setScale(low: number, high: number, unit: ScaleUnits) {
    this._addonInstance.setScale(low, high, unit);
  }

  extractionDone() {
    return this._addonInstance.extractionDone();
  }

  solvingDone() {
    return this._addonInstance.solvingDone();
  }

  finished() {
    return this._addonInstance.finished();
  }
}

export const FitsImageStatistic: FitsImageStatistic = addon.FitsImageStatistic;
